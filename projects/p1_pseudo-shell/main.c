/* Seth Tal
 * 04.04.2020
 * Psuedo-Shell: In this project we were tasked with creating a Pseudo-shell environment that
 *               that executes on the command line.
*/


#include "command.c"


void usage(int argc, char** argv) {
    if (!(argc >= 2 && argc <= 3)) {
        fprintf(stderr, "Usage: ./pseudo-shell <flag> <filename>\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {

    usage(argc, argv);

    return 0;
}