/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               that executes on the command line.
*/

/* -------------------------------------Preprocessor Directives--------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "command.c"

// ? TODO section
// TODO - write code to account for when user inserts tabs '\t', spaces, '\n', etc
// TODO - fflush(); whenever printing

// * Function declarations
void interactive_mode(int argc, char** argv);
void file_mode(int argc, char** argv);
void clean_up(char** input);
void error_handler();
void cmd_exec();


/* --------------------------------------------------------------------------------------------- */
/*                                      Usage and MAIN section                                   */
/* --------------------------------------------------------------------------------------------- */
/* IN: usage(int, char**), 
 * This function catches usage errors when 
 * running the program.
   OUT: none
*/
int usage(int argc, char** argv) {
    
    if (argc == 1) {
        return 0;
    } else {
        FILE* check_if_exists = fopen(argv[2], "r");
        char* flag = argv[1];
        // char* cf = "-f";
        if (strncmp("-f", flag, (size_t)2) == 0 && check_if_exists != NULL) {
            fclose(check_if_exists);
            return 1;
        } else if (strncmp("-f", flag, (size_t)2) != 0 && check_if_exists != NULL) {
            fprintf(stderr, "ERR: Invalid flag. ~ flag: %s\n", argv[1]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            fclose(check_if_exists);
            exit(EXIT_FAILURE);
        } else if (strncmp("-f", flag, (size_t)2) == 0 && check_if_exists == NULL) {
            fprintf(stderr, "ERR: invalid filename. ~ file: %s\n", argv[2]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            fclose(check_if_exists);
            exit(EXIT_FAILURE);
        } else if (strncmp("-f", flag, (size_t)2) != 0 && check_if_exists == NULL) {
            fprintf(stderr, "ERR: invalid flag and filename. ~ flag: %s, file: %s\n", argv[1], argv[2]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            fclose(check_if_exists);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char** argv) {

    int mode = usage(argc, argv);

    if (mode) { // if mode == 1, then file mode
        file_mode(argc, argv);
    } else { // else, then interactive mode
        interactive_mode(argc, argv);
    }

    return EXIT_SUCCESS;
}


/* --------------------------------------------------------------------------------------------- */
/*                               Source for File and Interactive Modes                           */
/* --------------------------------------------------------------------------------------------- */
void interactive_mode(int argc, char** argv) {
    printf("Running program in interactive mode...\n");
    /* Main Function Vars */
    int __exit_cmd = 0;      // tracks loop exit condition
    size_t size = 0;         // semantic / generic value for debugging purposes
    ssize_t num_lines = 0;   // holds output of getline
    char* input_buf = NULL;  // init input_buff
    char* curr_token = NULL; // holds current token durring processing

    while (!__exit_cmd) {
        /* print >>> then get the input string */
        printf(">>> ");

        // grab input data from user
        num_lines = getline(&input_buf, &size, stdin);

        // removes the new line character
        clean_up(&input_buf);

        /* if the user entered <exit> then exit the loop */
        if (strcmp(input_buf, "exit") == 0) {
            __exit_cmd = 1;
            printf("\n");
        }

        int i = 0; // counts num tokens

        /* Tokenize the input string */
        curr_token = strtok(input_buf, " ");
        
        /* Display each token */
        while (curr_token != NULL && __exit_cmd != 1) {
            if (i == 0)
                printf("\n");

            printf("T%d: %s", i, curr_token);
            i++;
            curr_token = strtok(NULL, " ");
            printf("\n");
        }
    }

    free(input_buf);
}

void file_mode(int argc, char** argv) {
    printf("Running program in file mode...");
    /* File Mode Vars */
    FILE* input = fopen(argv[2], "r");
    FILE* output = fopen("output.txt", "w");

    // tokening vars
    size_t size = 0;
    ssize_t num_lines = 0;
    char* input_buf = NULL;
    char* curr_token = NULL;

    num_lines = getline(&input_buf, &size, input);

    while (num_lines >= 0) {
        /* Tokenize the input line */
        int i = 0;
        curr_token = strtok(input_buf, " ");
        while (curr_token != NULL) {
            fprintf(output, "\n");
            fprintf(output, "T%d: %s", i, curr_token);
            i++;
            curr_token = strtok(NULL, " ");
        }

        num_lines = getline(&input_buf, &size, input);
    }

    /* free allocated memory */
    fclose(input);
    fclose(output);
    free(input_buf);
    printf(" done.\n");
}


/* --------------------------------------------------------------------------------------------- */
/*                                        Helper Functions                                       */
/* --------------------------------------------------------------------------------------------- */
void clean_up(char** input) {
    char* cleaned = *input;
    int   len = strlen(cleaned);
    cleaned[len - 1] = '\0';
    *input = cleaned;
}

void error_handler() {
    return;
}

void cmd_exec() {
    return;
}
