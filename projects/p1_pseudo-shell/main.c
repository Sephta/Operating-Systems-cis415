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

    // if (argc == 1) {
    //     printf("Running Pseudo-shell environment...\n");
    //     return 0;
    // } else if (!(argc >= 2 && argc <= 3)) {
    //     fprintf(stderr, "Usage: ./pseudo-shell\trun program in interactive mode.\n");
    //     fprintf(stderr, "\t-f <filename>\trun program in file mode.\n");
    //     exit(EXIT_FAILURE);
    // } else {
    //     FILE* check_if_exists = fopen(argv[2], "r");
    //     if (argv[1] != "-f" && check_if_exists != NULL) {
    //         fprintf(stderr, "ERR: Invalid flag. ~ flag: %s\n", argv[1]);
    //         exit(EXIT_FAILURE);
    //     } else if (argv[1] != "-f" && check_if_exists == NULL) {
    //         fprintf(stderr, "ERR: invalid flag and filename. ~ flag: %s\n", argv[1]);
    //         exit(EXIT_FAILURE);
    //     }
    //     fclose(check_if_exists);
    // }

    // return 0;
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
        size_t size = 255;
        char* input_buf[size];
        // malloc_buf(&input_buf, size);
        int num_chars = 0;
        printf("here3\n");
        while (strcmp((const char *)*input_buf, (const char *)"exit\n") != 0) {
            printf("here4\n");
            printf(">>> ");

            num_chars = getline(&input_buf[0], &size, stdin);

            printf("%s\n", *input_buf);

            printf("\n");
        }

        // printf("here1\n");
        // free(input_buf);
        // printf("here2\n");
    }

    return 0;
}