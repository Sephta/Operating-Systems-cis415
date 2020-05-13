/*
 * Seth Tal
 * 04.29.2020
 * Part 1 of Project 2: MCP Ghost in the shell
*/


/* ------------------------------------------- */
/*            #DEFINE and #INCLUDE             */
/* ------------------------------------------- */

#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


/* ------------------------------------------- */
/*              Helper Functions               */
/* ------------------------------------------- */

// This program catches usage errors
void usage(int argc, char** argv)
{
    // If the program does not have an input arguement
    if (2 > argc || argc > 2)
    {  // ERRORS...
        fprintf(stderr, "Error.  Invalid usage of program.\n");
        fprintf(stderr, "\tUsage: ./part<#> <filename>\n");
        fprintf(stderr, "\tAn input file is required to run the program.\n");
        exit(EXIT_FAILURE);
    }

    else
    {
        // check if file exists
        FILE* exists = fopen(argv[1], "r");
        if (exists == NULL)
        {
            fprintf(stderr, "Error. File \"%s\" either does not exists or cannot be opened.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
}

void printA(char** array, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (array[i] != NULL)
            fprintf(stdout, "arg[%d] : %s\n", i, array[i]);
        else if (array[i] == NULL)
            fprintf(stdout, "arg[n] : (null)\n");
    }
    fprintf(stdout, "\n");
}

void TokenizeProgram(char* toTokenize, char *** argArr, int arrLength)
{
    char** resultArgs;
    char* save = toTokenize;
    
    for (unsigned int i = 0; i < strlen(toTokenize); i++)
    {
        if (toTokenize[i] == '\n') {
            toTokenize[i] = '\0';
        }
    }
    
    resultArgs = (char**)malloc(sizeof(char*) * arrLength + 1);

    char* currToken = strtok_r(save, " ", &save);

    int i = 0;
    while (currToken != NULL)
    {
        resultArgs[i] = currToken;
        i++;
        currToken = strtok_r(save, " ", &save);
    }

    resultArgs[arrLength] = NULL;

    *argArr = resultArgs;
}

void GrabInput(char* filename, char*** programList, int* arrayLength)
{
    FILE* input = fopen(filename, "r");
    unsigned long int size;
    int len = 0;

    // for the purposes of this assingment bufsiz is an arbitrary size limit
    char** result = (char**)malloc(sizeof(char*) * BUFSIZ);

    for (unsigned long int i = 0; i < BUFSIZ; i++)
        result[i] = NULL;

    while (getline(&(result[len]), &size, input) != -1)
    {
        len++;
    }

    *programList = result;
    *arrayLength = len;

    fclose(input);
}

void ProcessInput(char** programs, int arrayLength)
{
    pid_t pids[arrayLength];
    char** args = NULL;
    // char* processName;
    int numSpaces = 1;

    printf("Parent process: %d\n",getpid());

    for (int currProc = 0; currProc < arrayLength; currProc++)
    {

        // Count the number of spaces in a given line
        for (unsigned long int j = 0; j < strlen(programs[currProc]); j++)
        {
            if (programs[currProc][j] == ' ')
                numSpaces++;
        }

        pids[currProc] = fork();

        if (pids[currProc] < 0)
        {
            fprintf(stderr, "Error. encountered an error when forking process. ID: %d", pids[currProc]);
            exit(EXIT_FAILURE);
        }

        if (pids[currProc] == 0)
        {
            printf("Child proc: %d, with id: %d\n", currProc+1, getpid());
            TokenizeProgram(programs[currProc], &args, numSpaces);
            if (execvp(args[0], args) < 0)
            {
                fprintf(stderr, "Error. an error occured when running program from Child[%d]: (%d)\n\n", currProc+1, getpid());
                free(args);
                exit(-1);
            }
            free(args);
            exit(-1);
        }

        for (int i = 0; i < arrayLength; i++)
        {
            wait(&pids[i]);
        }

        // reset number of items to tokenize
        numSpaces = 1;
    }
}


/* --------------------------------------------------------------------------------------------- */
/*                                               Main                                            */
/* --------------------------------------------------------------------------------------------- */

int main (int argc, char** argv)
{
    printf("Running program...\n");
    printf("\n\n");

    // Catch any program usage errors
    usage(argc, argv);

    char** programs = NULL;
    int len = 0;

    GrabInput(argv[1], &programs, &len);

    ProcessInput(programs, len);

    printf("\n\n");
    printf("Done.\n");

    free(programs);

    return 0;
}
