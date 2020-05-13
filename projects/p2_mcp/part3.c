/*
 * Seth Tal
 * 04.29.2020
 * Part 3 of Project 2: MCP Ghost in the shell
*/


/* ------------------------------------------- */
/*            #DEFINE and #INCLUDE             */
/* ------------------------------------------- */

// This enables the use of sigset_t
// #define _POSIX_C_SOURCE 1
// #define __USE_XOPEN_EXTENDED 1
// #define __USE_XOPEN2K8 1

// #include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


// VOLATILE FLAGS

// static volatile sig_atomic_t stop_flag = 0;
// static volatile sig_atomic_t cont_flag = 0;
static volatile sig_atomic_t reset_alarm_flag = 1;


void usage(int argc, char** argv);
void GrabInput(char* filename, char*** programList, int* size);
void printA(char** array, int length);
void ProcessInput(char** programs, int arrayLength, pid_t pids[], pid_t parent);
void TokenizeProgram(char* toTokenize, char*** argArr, int arrLength);


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

// void handler_sigusr1(int sig) {}

void handler_alarm(int sig)
{
    // stop_flag = 1;
    // cont_flag = 1;
    reset_alarm_flag = 1;
}

/* ------------------------------------------- */
/*              PROGRAM PROCESSOR              */
/* ------------------------------------------- */

void ProcessInput(char** programs, int arrayLength, pid_t pids[], pid_t parent)
{
    char** args = NULL;
    int numSpaces = 1;

    // Parent id
    printf("Parent Proccess ID: (%d), will have %d children proccesses.\n\n", parent, arrayLength);

    printf("/* Setting up SIGSET for child ---------------- */\n");
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
        if (getpid() == parent)
            printf("In Parent Proc(%d)... Child(%d) forked -> pids[%d].\n", getpid(), pids[program], program+1);

        // if an error occured when forking this program
        if (pids[program] < 0)
        {
            fprintf(stderr, "Error. encountered an error when forking process. ID: %d", pids[program]);
            exit(EXIT_FAILURE);
        }

        // if program is a child process
        if (pids[program] == 0)
        {
            // printf("\n\tEstablishing sigwait for Child[%d]: %d\n\n", program+1, getpid());

            if (sigwait(&sigset, &sig) < 0)
            {
                fprintf(stderr, "Error. error with sigwait in Child[%d]: (%d)\n", program+1, getpid());
                exit(EXIT_FAILURE);
            }

            if (sig == SIGUSR1)
            {
                printf("Child[%d]: (%d) recieved SIGUSR1 signal.\n\n", program+1, getpid());
                printf("start --- Running Child[%d], with id: (%d).\n", program+1, getpid());
                TokenizeProgram(programs[program], &args, numSpaces);
                if (execvp(args[0], args) < 0)
                {
                    fprintf(stderr, "Error. an error occured when running program from Child[%d]: (%d)\n\n", program+1, getpid());
                    exit(-1);
                }
                exit(-1);
            }
        }

        // reset number of items to tokenize
        numSpaces = 1;

        // free any malloc'd memory for next itteration
        free(args);
    }
}

void Parent_Proccessor(pid_t pids[], pid_t parent, int numProc)
{
    printf("\n/* Setting up SIGSET for parent --------------- */\n");
    // Signal setup for SIGUSR1
    int sig;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    if (sigismember(&sigset, SIGALRM))
        printf("/*   SIGALRM added as member of parent sigset.  */\n");

    // if this is the parent process
    if (getpid() == parent)
    {
        // * Send SIGUSR1 signal to child processes
        for (int i = 0; i < numProc; i++)
        {
            // send SIGUSR1 signal to the child process
            if (kill(pids[i], SIGUSR1) < 0) 
            {
                fprintf(stderr, "Error. an error occured when signaling child process. (SIGUSR1)\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("\nSending SIGUSR1 to Child[%d]: (%d)\n\n", i+1, pids[i]);
                // alarm(1);
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
        }

        kill(pids[0], SIGCONT);

        int childStatus[numProc];
        for (int i = 0; i < numProc; i++)
            childStatus[i] = 0;

        int numDone = 0;
        int status;
        int currentProc = 0;
        pid_t waitPid;
        while (1)
        {
            if (childStatus[currentProc] == 0)
            {
                alarm(1);

                if (sigwait(&sigset, &sig) < 0)
                {
                    fprintf(stderr, "Error. error with sigwait in Parent: (%d)\n", getpid());
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("\n\nTelling parent process to wait. currProc: %d\n\n", currentProc+1);
                }
            }
            
            // if (reset_alarm_flag == 1)
            // {
            if (childStatus[currentProc] != 1)
            {
                if (kill(pids[currentProc], SIGSTOP) < 0)
                {
                    fprintf(stderr, "Error. an error occured when signaling child process (in Scheduler). (SIGSTOP)\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    printf("\tScheduler Stopping Child[%d]: (%d).\n\n", currentProc+1, pids[currentProc]);
                }
            }
            reset_alarm_flag = 0;
            if (currentProc+1 >= numProc)
            {
                if (childStatus[0] != 1)
                {
                    if (kill(pids[0], SIGCONT) < 0)
                    {
                        fprintf(stderr, "Error. an error occured when signaling child process (in Scheduler). (SIGCONT):1\n");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        printf("\tScheduler continuing Child[%d]: (%d).\n\n", 1, pids[0]);
                    }
                }
            }
            else
            {
                if (childStatus[currentProc+1] != 1)
                {
                    if (kill(pids[currentProc+1], SIGCONT) < 0)
                    {
                        fprintf(stderr, "Error. an error occured when signaling child process (in Scheduler). (SIGCONT):2\n");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        printf("\tScheduler continuing Child[%d]: (%d).\n\n", currentProc+2, pids[currentProc+1]);
                    }
                }
            }
            // }

            // Check if any procs are done this itteration of while(1)
            for (int i = 0; i < numProc; i++)
            {
                if (childStatus[i] != 1)
                    waitPid = waitpid(pids[i], &status, WNOHANG | WUNTRACED | WCONTINUED);
                else
                    continue;
                if (waitPid == -1)
                {
                    // printf("Status: %d\n", WIFEXITED(status));
                    fprintf(stderr, "Error. error when using waitpid() on Child[%d]: (%d)\n", i+1, pids[i]);
                    exit(EXIT_FAILURE);
                }
                if (WIFEXITED(status) > 0)
                {
                    childStatus[i] = WIFEXITED(status);
                }
            }



            // Reset which process we are looking at to the first process
            // if we had just looked at the last process
            if (currentProc+1 >= numProc)
                currentProc = 0;
            else
                currentProc++;

            // int check = currentProc;
            // while (1)
            // {
            //     if ((check+1) >= numProc)
            //     {
            //         if (childStatus[0] == 0)
            //         {
            //             currentProc = 0;
            //             break;
            //         }
            //     }
            //     else
            //     {
            //         if (childStatus[check+1] == 0)
            //         {
            //             currentProc = check+1;
            //             break;
            //         }
            //     }

            //     if (check+1 >= numProc)
            //         check = 0;
            //     else
            //         check++;
            // }

            // Check children
            for (int i = 0; i < numProc; i++)
            {
                // printf("Child[%d]: %d, has status: %d\n", i+1, pids[i], childStatus[i]);
                if (childStatus[i] == 1)
                    numDone++;
            }

            // printf("NumDone: %d\n\n", numDone);

            if (numDone >= numProc)
                break;
            else
                numDone = 0;
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

    pid_t parentID = getpid();

    printf("Registering Signal handler for SIGALRM\n\n");
    signal(SIGALRM, handler_alarm);

    // Catch any program usage errors
    usage(argc, argv);

    char** programs = NULL;
    int len = 0;

    GrabInput(argv[1], &programs, &len);
    
    pid_t pids[len];

    ProcessInput(programs, len, pids, parentID);

    Parent_Proccessor(pids, parentID, len);

    printf("\n\n");
    printf("Done.\n");

    free(programs);

    return 0;
}
