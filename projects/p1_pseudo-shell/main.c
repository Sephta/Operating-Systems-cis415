/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               that executes on the command line.
*/

/* Q's for prof
* Ask about -f flag, the error I have, and how this might be properly implemented
* Ask about tokenizing the string into readable commands
* Ask about overall project structure
*/

/* -------------------------------------Preprocessor Directives--------------------------------- */
#include <string.h>
#include <sys/types.h>
#include "command.c"
/* --------------------------------------------------------------------------------------------- */

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
            return 1;
        } else if (strncmp("-f", flag, (size_t)2) != 0 && check_if_exists != NULL) {
            fprintf(stderr, "ERR: Invalid flag. ~ flag: %s\n", argv[1]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            exit(EXIT_FAILURE);
        } else if (strncmp("-f", flag, (size_t)2) == 0 && check_if_exists == NULL) {
            fprintf(stderr, "ERR: invalid filename. ~ file: %s\n", argv[2]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            exit(EXIT_FAILURE);
        } else if (strncmp("-f", flag, (size_t)2) != 0 && check_if_exists == NULL) {
            fprintf(stderr, "ERR: invalid flag and filename. ~ flag: %s, file: %s\n", argv[1], argv[2]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            exit(EXIT_FAILURE);
        }
        fclose(check_if_exists);
    }
}

// TODO - move interactive mode and file mode to there own functions
void interactive_mode() {}

void file_mode() {}
/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
int main(int argc, char** argv) {

    // TODO - write code to account for when user inserts tabs '\t', spaces, '\n', etc
    // TODO - fflush(); whenever printing

    int mode = usage(argc, argv);

    if (mode) { // if mode == 1, then file mode
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
        free(input_buf);
        fclose(input);
        fclose(output);
        printf(" done.\n");
    } else { // else, then interactive mode
        printf("Running program in interactive mode...\n");
        /* Main Function Vars */
        int __exit_cmd = 0;      // tracks loop exit condition
        int __empty_input = 0;   // bool tracking if input is empty
        size_t size = 0;         // semantic / generic value for debugging purposes
        ssize_t num_lines = 0;   // holds output of getline
        char* input_buf = NULL;  // init input_buff
        char* curr_token = NULL; // holds current token durring processing

        while (!__exit_cmd) {
            /* print >>> then get the input string */
            printf(">>> ");

            // grab input data from user
            num_lines = getline(&input_buf, &size, stdin);

            // ! bellow line is for debugging
            // printf("nc: %lu\n", num_chars);

            if (num_lines <= (size_t)1)
                __empty_input = 1;

            /* if the user entered <exit> then exit the loop */
            if (strcmp(input_buf, "exit\n") == 0) {
                __exit_cmd = 1;
                printf("\n");
            }
            
            /* Tokenize the input string */
            // printf("\n");
            int i = 0;
            curr_token = strtok(input_buf, " ");
            /* Display each token */
            while (curr_token != NULL && __exit_cmd != 1 && __empty_input != 1) {
                printf("\n");
                printf("T%d: %s", i, curr_token);
                i++;
                curr_token = strtok(NULL, " ");
            }
        }

        free(input_buf);
    }

    return 0;
}
/* --------------------------------------------------------------------------------------------- */
