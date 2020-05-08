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


/* ------------------------------------------- */
// Function Declarations for this program

void mallocProgramList(char*** list, int counter);
int firstPass(char** argv);
char** secondPass(char** argv, char** programs, int count);

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

/* Psuedocode from project specs
* for i=0 -> numprograms-1 {
*     read program[i] and its args[i] from the file;
*     pid[i] = fork();
*     if (pid[i] < 0) {
*         handle this error appropriately
*     }
*     if (pid[i] == 0) {
*         exec(program[i], arguements[i]);
*         exit(-1);
*     }
* }
* for i=0 -> numprograms-1 {
*     wait(pid[i]);
* }
*/
int main (int argc, char** argv)
{
    printf("Running prog...\n");
    // Catch any program usage errors
    usage(argc, argv);

    // counts the number of programs
    int pCounter = firstPass(argv);

    // ! printf("count: %d\n", pCounter);

    // char* programs[pCounter];

    // ? Allocate memory for list of programs and arguements
    // mallocProgramList(&programList, pCounter);

    char** programList = NULL;

    secondPass(argv, programList, pCounter);
    for (int i = 0; i < pCounter; i++)
    {
        printf("here\n");
        printf("%s", programList[i]);
    }
    printf("\n");

    // Array of PIDs
    // pid_t pidArr[pCounter];

    // pidArr[pCounter] = fork();
    // if (pidArr[pCounter] < 0)
    // {
    //     // TODO - handle this error
    // }
    // if (pidArr[pCounter] == 0)
    // {
    //     //exec the program and args
    //     exit(-1);
    // }

    free(programList);

    return 0;
}


/* ------------------------------------------- */
// Helper Functions

void mallocProgramList(char*** list, int counter)
{
    *list = (char**)malloc(sizeof(char*) * counter);
}

int firstPass(char** argv)
{
    FILE* input = fopen(argv[1], "r");
    unsigned long int size = 0;
    signed long int getLineFlag = 0;
    char* inputBuf = NULL;

    int count = 0;

    getLineFlag = getline(&inputBuf, &size, input);
    while (getLineFlag >= 0)
    {
        count++;
        getLineFlag = getline(&inputBuf, &size, input);
    }

    // printf("count: %d\n", count);
    free(inputBuf);
    fclose(input);

    return count;
}

char** secondPass(char** argv, char** programs, int count)
{
    FILE* input = fopen(argv[1], "r");
    unsigned long int size = 0;
    signed long int getLineFlag = 0;
    char* inputBuf = NULL;

    // mallocProgramList(&programs, count);
    programs = (char**)malloc(sizeof(char*) * count);

    int i = 0;
    getLineFlag = getline(&inputBuf, &size, input);
    while (getLineFlag >= 0)
    {
        printf("here%d\n", i+1);
        programs[i] = inputBuf;
        // printf("%s", programs[i]);
        if (i < count)
            i++;
        getLineFlag = getline(&inputBuf, &size, input);
    }

    free(inputBuf);
    fclose(input);
}
