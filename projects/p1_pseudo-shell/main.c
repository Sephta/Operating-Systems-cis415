/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               that executes on the command line.
*/


#include <string.h>
#include "command.c"

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
        char* cf = "-f";
        if (flag == cf && check_if_exists != NULL) {
            return 2;
        } else if (flag != cf && check_if_exists != NULL) {
            fprintf(stderr, "ERR: Invalid flag. ~ flag: %s\n", argv[1]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            exit(EXIT_FAILURE);
        } else if (flag != cf && check_if_exists == NULL) {
            fprintf(stderr, "ERR: invalid flag and filename. ~ flag: %s\n", argv[1]);
            fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
            fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
            exit(EXIT_FAILURE);
        }
        fclose(check_if_exists);
    }

    return 0;
}

void malloc_buf(char** buf, size_t size) {
    *buf = (char *)malloc(sizeof(char) * size);
}

int main(int argc, char** argv) {

    int active_file = usage(argc, argv);

    if (active_file) { // file mode
        printf("file mode initializing...\n");
    } else { // interactive mode
        printf("interactive mode initializing...\n");
        size_t size = 255;       // semantic / generic value for debugging purposes
        size_t num_chars = 0;    // holds output of getline
        char* input_buf = NULL;  // init input_buff
        char* curr_token = NULL; // holds current token durring processing
        
        // setup the input buffer to hold the line entered by the user
        malloc_buf(&input_buf, size);

        while (strcmp(input_buf, "exit\n") != 0) {
            printf(">>> ");

            // grab input date from user
            num_chars = getline(&input_buf, &size, stdin);
            
            // process tokens
            printf("\n");
            int i = 0;
            curr_token = strtok(input_buf, " ");
            while (curr_token != NULL) {
                printf("T%d: %s\n", i, curr_token);
                i++;
                curr_token = strtok(NULL, " ");
            }
        }

        free(input_buf);
    }

    return 0;
}