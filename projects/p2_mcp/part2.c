/*
 * Seth Tal
 * 04.29.2020
 * Part 2 of Project 2: MCP Ghost in the shell
*/


/* ------------------------------------------- */
/*            #DEFINE and #INCLUDE             */
/* ------------------------------------------- */

// This enables the use of sigset_t
#define _POSIX_C_SOURCE 1

#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


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

/* ------------------------------------------- */
/*               SIGNAL HANDLERS               */
/* ------------------------------------------- */

void handler_sigusr1(int sig)
{
    if (sig == SIGUSR1)
    {
        fprintf(stdout, "Signal SIGUSR1 recieved by process: %d.", getpid());
    }
    else
    {
        fprintf(stderr, "Error. sigusr1 handler recieved wrong signal\n");
        exit(EXIT_FAILURE);
    }
}

/* ------------------------------------------- */
/*              PROGRAM PROCESSOR              */
/* ------------------------------------------- */

void ProcessInput(char** programs, int arrayLength)
{
    pid_t pids[arrayLength];
    char** args = NULL;
    int numSpaces = 1;

    // Parent id
    int parentID = getpid();
    printf("Parent Proccess ID: (%d), will have %d children proccesses\n\n", parentID, arrayLength);

    printf("/* Setting up SIGSET -------------------------- */\n");
    printf("/*                                              */\n");
    // Signal setup for SIGUSR1
    int sig;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    if (sigismember(&sigset, SIGUSR1))
        printf("/*\tSIGUSR1 added as member of sigset\t*/\n");

    printf("/*                                              */\n");
    printf("/* -------------------------------------------- */\n\n");

    for (int program = 0; program < arrayLength; program++)
    {
        // Count the number of spaces in a given line (this denotes number of tokens per line)
        for (unsigned long int j = 0; j < strlen(programs[program]); j++)
        {
            if (programs[program][j] == ' ')
                numSpaces++;
        }

        // create new child process
        pids[program] = fork();
        if (getpid() == parentID)
            printf("In Parent Proc(%d)... Child(%d) forked -> pids[%d].\n\n", getpid(), pids[program], program+1);

        // if an error occured when forking this program
        if (pids[program] < 0)
        {
            fprintf(stderr, "Error. encountered an error when forking process. ID: %d", pids[program]);
            exit(EXIT_FAILURE);
        }

        // if program is a child process
        if (pids[program] == 0)
        {
            printf("\tEstablishing sigwait for Child[%d]: %d\n\n", program+1, getpid());

            if (sigwait(&sigset, &sig) < 0)
            {
                fprintf(stderr, "Error. error with sigwait\n");
                exit(EXIT_FAILURE);
            }

            if (sig == SIGUSR1)
            {
                printf("start --- Running Child[%d], with id: (%d)\n", program+1, getpid());
                TokenizeProgram(programs[program], &args, numSpaces);
                if (execvp(args[0], args) < 0)
                {
                    fprintf(stderr, "Error. an error occured when running program from Child[%d]: (%d)\n\n", program+1, getpid());
                    free(args);
                    exit(-1);
                }
                free(args);
                exit(-1);
            }
        }

        // reset number of items to tokenize
        numSpaces = 1;
    }

    // if this is the parent process
    if (getpid() == parentID)
    {
        // * Send SIGUSR1 signal to child processes
        for (int i = 0; i < arrayLength; i++)
        {
            // send SIGUSR1 signal to the child process
            printf("\n\tSending SIGUSR1 to Child[%d]: %d\n\n", i+1, pids[i]);
            if (kill(pids[i], SIGUSR1) < 0) 
            {
                fprintf(stderr, "Error. an error occured when signaling child process. (SIGUSR1)\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Child[%d]: (%d), should be recieving SIGUSR1 signal.\n\n", i+1, pids[i]);
            }

            // Telling child procs to stop...
            if (kill(pids[i], SIGSTOP) < 0)
            {
                fprintf(stderr, "Error. an error occured when signaling child process. (SIGSTOP)\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("\tStopping Child[%d]: (%d).\n\n", i+1, pids[i]);
            }

            // Telling child procs to continue...
            if (kill(pids[i], SIGCONT) < 0)
            {
                fprintf(stderr, "Error. an error occured when signaling child process. (SIGCONT)\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("\tContinuing Child[%d]: (%d).\n\n", i+1, pids[i]);
            }

            // Telling parent to wait for this child process before continuing
            int status;
            wait(&status);
        }
    }
}


/* --------------------------------------------------------------------------------------------- */
/*                                               Main                                            */
/* --------------------------------------------------------------------------------------------- */

int main (int argc, char** argv)
{
    printf("Running...\n");
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
