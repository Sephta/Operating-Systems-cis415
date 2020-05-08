/*
Seth Tal
04.24.2020
Lab4 - code copied from the lab, was originally written by Grayson Guan
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
    pid_t pid;

    printf("Main start, pid here is: %d\n", getpid());

    pid = fork();

    if (pid == 0) {
        printf("This is the child process. The pid here is: %d, my parent is: %d\n", getpid(), getppid());
        printf("My status is: %d\n\n", pid);
        // if (execv("./helloWorld", NULL) < 0) {
        //     perror("execv");
        // }
        sleep(2);
    } else {
        wait(0);
        printf("Main exiting, my pid is: %d\n", getpid());
    }

    return 1;
}