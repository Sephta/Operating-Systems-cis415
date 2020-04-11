/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               executes on the command line.
*/


#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void listDir()
{
    struct dirent *dirent;

    DIR *dir = opendir(".");
    
    if (dir != NULL) {
        while ((dirent=readdir(dir)) != NULL) {
            write(1, dirent->d_name, strlen(dirent->d_name));
            write(1, " ", strlen(" "));
        }
        write(1, "\n", strlen("\n"));
    } else {
        write(2, "Error! could not open current directory.\n", 41);
        return;
    }

    free(dir);
}

void showCurrentDir()
{
    long size;
    char* ptr = NULL;

    size = pathconf(".", _PC_PATH_MAX);
    char buf[(size_t) size];

    if (buf != NULL)
        ptr = getcwd(buf, (size_t)size);
    
    write(1, ptr, strlen(ptr));
    write(1, "\n", strlen("\n"));
}

void makeDir(char *dirName)
{
    int flag;
    flag = mkdir(dirName, 0777);

    if (flag != -1) {
        write(1, "Directory created.\n", strlen("Directory created\n"));
    } else {
        write(2, "Error! directory could not be created.\n", strlen("Error! directory could not be created.\n"));
        return;
    }
}

void changeDir(char *dirName)
{
    int flag;
    flag = chdir((const char*)dirName);

    if (flag != -1)
        write(1, "Directory changed.\n", strlen("Directory changed.\n"));
    else {
        write(2, "Error! directory could not be changed.\n", strlen("Error! directory could not be changed.\n"));
        return;
    }
}

void copyFile(char *sourcePath, char *destinationPath)
{
    int srcFD, dstFD, flagRead, flagWrite;
    int BUF_SIZE = 1024;

    // 256 bellow is an arbitrary constant
    char* buf[BUF_SIZE];

    srcFD = open(sourcePath, O_RDONLY);
    if (srcFD < 0) {
        write(2, "Error! something went wrong when obtaining file descripter for source.\n", strlen("Error! something went wrong when obtaining file descripter for source.\n"));
        return;
    }

    dstFD = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (dstFD < 0) {
        write(2, "Error! something went wrong when obtaining file descripter for destination.\n", strlen("Error! something went wrong when obtaining file descripter for destination.\n"));
        return;
    }

    while ((flagRead = read(srcFD, buf, BUF_SIZE)) > 0) {
        if (write(dstFD, buf, flagRead) != flagRead) {
            write(2, "Error! an error occured when opening file.\n", strlen("Error! an error occured when opening file.\n"));
            return;
        }
    }

    if (flagRead == -1) {
        write(2, "Error! an error occured when reading file.\n", strlen("Error! an error occured when reading file.\n"));
        return;
    }

    if (close(srcFD) == -1) {
        write(2, "Error! an error occured when closing source file.\n", strlen("Error! and error occured when closing source file.\n"));
        return;
    }

    if (close(dstFD == -1)) {
        write(2, "Error! an error occured when closing destination file.\n", strlen("Error! an error occured when closing destination file.\n"));
        return;
    }
}

void moveFile(char *sourcePath, char *destinationPath)
{

}

void deleteFile(char *filename)
{

}

void displayFile(char *filename)
{
    
}
