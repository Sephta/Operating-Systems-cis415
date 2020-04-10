/*
* Description: In this lab we processed user input from both a file and stdin to split string into tokens
*              to be processed for use later in Project 1
*
* Author: Seth Tal
*
* Date: 04.07.2020
*
* Notes:
* 1. this was essentially a copy-paste from the work I had already done for the project so
*    some of the comments are the same, and I have like one or two debugging related statements here
*/

/* -------------------------------------Preprocessor Directives--------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* --------------------------------------------------------------------------------------------- */

void malloc_buf(char** buf, size_t size) {
    *buf = (char *)malloc(sizeof(char) * size);
}

/* -----------------------------------------Program Main---------------------------------------- */
int main() {
    setbuf(stdout, NULL);

    /* Main Function Vars */
    int __exit_cmd = 0;      // tracks loop exit condition
    int __empty_input = 0;   // bool tracking if input is empty
    size_t size = 255;       // semantic / generic value for debugging purposes
    size_t num_chars = 0;    // holds output of getline
    char* input_buf = NULL;  // init input_buff
    char* curr_token = NULL; // holds current token durring processing

    /* Allocate memory for input buffer. */
    malloc_buf(&input_buf, size);

    /* main run loop */
    while (!__exit_cmd) {
        /* print >>> then get the input string */
        printf(">>> ");

        // grab input data from user
        num_chars = getline(&input_buf, &size, stdin);

        // ! bellow line is for debugging
        // printf("nc: %lu\n", num_chars);

        if (num_chars <= (size_t)1)
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

    /* free allocated memory */
    free(input_buf);

    return 1;
}
/* --------------------------------------------------------------------------------------------- */