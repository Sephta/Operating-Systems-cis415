/*
 * Seth Tal
 * 04.29.2020
 * Part 1 of Project 2: MCP Ghost in the shell
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


/* ------------------------------------------- */
// Function Declarations for this program
void GrabInput(char* filename, char*** programList, int* size);
void printA(char** array, int length);
void ProcessInput(char** programs, int arrayLength);
void TokenizeProgram(char* toTokenize, char*** argArr, int arrLength);

/* ------------------------------------------- */
// Main Program functions

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

int main (int argc, char** argv)
{
    printf("Running prog...\n");
    // Catch any program usage errors
    usage(argc, argv);

    char** programs = NULL;
    int len = 0;

    GrabInput(argv[1], &programs, &len);

    ProcessInput(programs, len);

    free(programs);

    return 0;
}


/* ------------------------------------------- */
// Helper Functions

void printA(char** array, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (array[i] != NULL)
            fprintf(stdout, "arg[%d] : %s\n", i, array[i]);
        else if (array[i] == NULL)
            fprintf(stdout, "arg[n] : (null)\n", i);
    }
    fprintf(stdout, "\n");
}

void TokenizeProgram(char* toTokenize, char *** argArr, int arrLength)
{
    char** resultArgs;
    char* save = toTokenize;
    
    for (int i = 0; i < strlen(toTokenize); i++)
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
    int numSpaces = 1;

    for (int program = 0; program < arrayLength; program++)
    {

        // Count the number of spaces in a given line
        for (unsigned long int j = 0; j < strlen(programs[program]); j++)
        {
            if (programs[program][j] == ' ')
                numSpaces++;
        }

        TokenizeProgram(programs[program],/* &currProgram,*/ &args, numSpaces);

        // printf("path: %s\n", args[0]);
        // printA(args, numSpaces + 1);

        pids[program] = fork();
        if (pids[program] < 0)
        {
            fprintf(stderr, "Error. encountered an error when forking process. ID: %d", pids[program]);
            exit(EXIT_FAILURE);
        }
        if (pids[program] == 0)
        {
            execvp(args[0], args);
            exit(-1);
        }

        // reset number of items to tokenize
        numSpaces = 1;

        // free any malloc'd memory for next itteration
        free(args);
    }

    for (int i = 0; i < arrayLength; i++)
    {
        wait(&pids[i]);
    }
}
