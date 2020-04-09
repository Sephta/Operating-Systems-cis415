/*
* Description: In this lab we processed user input from both a file and stdin to split string into tokens
*              to be processed for use later in Project 1
*
* Author: Seth Tal
*
* Date: 04.07.2020
*
* Notes:
* 1. I needed to import sys/types.h in order to use ssize_t which is a signed type that can represent -1
* 2. 
*/

/* -------------------------------------Preprocessor Directives--------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
/* -----------------------------------------Program Main---------------------------------------- */
int main(int argc, char** argv) {
    setbuf(stdout, NULL);

    if (argc == 3) {
        /* File Mode Vars */
        FILE* input = fopen(argv[2], "r");
        FILE* output = fopen("output.txt", "w");

        // tokening vars
        size_t size = 0;
        ssize_t num_chars = 0;
        char* input_buf = NULL;
        char* curr_token = NULL;

        num_chars = getline(&input_buf, &size, input);

        while (num_chars >= 0) {
            /* Tokenize the input line */
            int i = 0;
            curr_token = strtok(input_buf, " ");
            while (curr_token != NULL) {
                fprintf(output, "\n");
                fprintf(output, "T%d: %s", i, curr_token);
                i++;
                curr_token = strtok(NULL, " ");
            }

            num_chars = getline(&input_buf, &size, input);
        }

        /* free allocated memory */
        free(input_buf);
        fclose(input);
        fclose(output);

        return 1;
    } else {
        return 0;
    }
}
/* --------------------------------------------------------------------------------------------- */