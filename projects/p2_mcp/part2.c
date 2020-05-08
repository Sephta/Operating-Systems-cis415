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
// #include <bits/types/__sigset_t.h>


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

void GetProgramName(const pid_t id, char name[])
{
    // This function is designed after user fclairamb on GitHub
    // Link: https://gist.github.com/fclairamb/a16a4237c46440bdb172#file-get_ppid_and_name-c-L23

    // char *result = (char *)malloc(sizeof(char) * BUFSIZ);

    char buf[BUFSIZ];

    sprintf(buf, "/proc/%d/cmdline", id);

    FILE* file = fopen(buf, "r");

    if (file)
    {
        signed long int size;
        size = fread(name, sizeof(char), sizeof(buf), file);

        if (size != -1)
        {
            if (name[size - 1] == '\n')
            {
                name[size - 1] = '\0';
            }
        }
        else if (size == -1)
        {
            fprintf(stderr, "Error. an error occurred when reading from file: %s", buf);
            exit(EXIT_FAILURE);
        }
        fclose(file);
    }
    // *name = result;
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
    // char* processName;
    int numSpaces = 1;

    // Parent id
    int parentID = getpid();

    // Signal setup for SIGUSR1
    int sig;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGSTOP);
    sigaddset(&sigset, SIGCONT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    if (sigismember(&sigset, SIGUSR1))
        printf("SIGUSR1 is a member of sigset\n\n");
    
    if (sigismember(&sigset, SIGSTOP))
        printf("SIGSTOP is a member of sigset\n\n");
    
    if (sigismember(&sigset, SIGCONT))
        printf("SIGCONT is a member of sigset\n\n");

    for (int program = 0; program < arrayLength; program++)
    {
        // char processName[BUFSIZ];

        // Count the number of spaces in a given line (this denotes number of tokens per line)
        for (unsigned long int j = 0; j < strlen(programs[program]); j++)
        {
            if (programs[program][j] == ' ')
                numSpaces++;
        }
        // -----------------------------------------------------------------------------------

        // create new child process
        pids[program] = fork();

        // if an error occured when forking this program
        if (pids[program] < 0)
        {
            fprintf(stderr, "Error. encountered an error when forking process. ID: %d", pids[program]);
            exit(EXIT_FAILURE);
        }

        // if program is a child process
        if (pids[program] == 0)
        {
            printf("Child proc: %d, with id: %d\n", program+1, getpid());
            TokenizeProgram(programs[program], &args, numSpaces);
            execvp(args[0], args);
            exit(-1);

            // // SIGWAIT -----------------------------------------
            // int execSignal = sigwait(&sigset, &sig);
            // printf("signal: %d\n", sig);
            // if (execSignal < 0)
            // {
            //     fprintf(stderr, "Error. error with sigwait\n");
            //     exit(EXIT_FAILURE);
            // }
            // // -------------------------------------------------

            // if (sig == SIGUSR1)
            // {
            //     // Tokenize each line to make list of arguements for exec call
            //     TokenizeProgram(programs[program], &args, numSpaces);
            //     execvp(args[0], args);
            //     exit(-1);
            // }
        }

        if (parentID = getpid())
        {
            wait(&parentID);

            // // SIGWAIT -----------------------------------------
            // int execSignal = sigwait(&sigset, &sig);
            // printf("signal: %d\n", sig);
            // if (execSignal < 0)
            // {
            //     fprintf(stderr, "Error. error with sigwait\n");
            //     exit(EXIT_FAILURE);
            // }
            // // -------------------------------------------------
        }

        // * Send SIGUSR1 signal to child processes
        // for (int i = 0; i < arrayLength; i++)
        // {
        //     // printf("here\n");
        //     int err = 0;
        //     if (pids[i] == 0)
        //         if ((err = kill(pids[i], SIGUSR1)) < 0) 
        //         {
        //             fprintf(stderr, "Error. an error occured when signaling child process\n");
        //             exit(EXIT_FAILURE);
        //         }
        //         else
        //         {
        //             printf("Process started with ID: %d, with parent ID: %d\n", getpid(), getppid());
        //         }
        // }

        // int ppid = getpid();
        // printf("TEST: %d\n", ppid);
        // wait(&ppid);

        // reset number of items to tokenize
        numSpaces = 1;

        // free any malloc'd memory for next itteration
        free(args);
    }

    for (int i = 0; i < arrayLength; i++)
    {
        if (pids[i] == 0)
        {
            // SIGWAIT -----------------------------------------
            int execSignal = sigwait(&sigset, &sig);
            printf("signal: %d\n", sig);
            if (execSignal < 0)
            {
                fprintf(stderr, "Error. error with sigwait\n");
                exit(EXIT_FAILURE);
            }
            // -------------------------------------------------
        }
    }

    // for (int i = 0; i < arrayLength; i++)
    // {
    //     printf("proc: %d\n", pids[i]);
    // }
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