/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               executes on the command line.
*/


#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
// ! #include <string.h>


// Learning how to write to stdout
// 0 == stdin, 1 == stdout, 2 == stderr
/*  
    char* helloWorld = "hello world\n";
    if (write(1, (const void*)helloWorld, 13) != 13) {
        write(2, "Error! write to stdout had an error.\n", 38);
        return;
    }
*/


void listDir()
{
    struct dirent *dirent;

    DIR *dir = opendir(".");
    
    if (dir != NULL) {
        while ((dirent=readdir(dir)) != NULL) {
            // write(1, dirent->d_name, 256);
            // ! if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, ".."))
            printf("%s ", dirent->d_name);
        }
        printf("\n");
        // write(1, "\n", 1);
    } else {
        // write(2, "Error! could not open current directory.\n", 41);
        fprintf(stdout, "Error: encountered error when trying to open current directory.\n");
        return;
    }
}

void showCurrentDir()
{
    
}

void makeDir(char *dirName) {}

void changeDir(char *dirName) {}

void copyFile(char *sourcePath, char *destinationPath) {}

void moveFile(char *sourcePath, char *destinationPath) {}

void deleteFile(char *filename) {}

void displayFile(char *filename) {}
