# Psuedo Shell Environmnet

## Project 1 for CIS 415 (Operating Systems) @ University of Oregon

In this project I was tasked with utilizing various Unix system calls to emulate common behavior in command line interfaces. The program was developed in C, inside of an arch linux virtual machine.

The shell can be started in one of two modes: A) File mode, B) Interactive mode. To run the shell
interactively, simply type: ./pseudo-shell. Upon executing, you will see a shell-like CLI that is
ready to receive commands. This CLI sits at the ready state, indicated by the prompt “>>>”, until 
the user inputs a command string and presses enter. In this shell mode, the user interacts with 
the shell by typing in commands one line at a time and pressing enter. The shell then executes 
those commands and displays any output if necessary (some commands execute with no output such as 
rm or mv).

### Emulated Commands

* pwd - shows the current directory
* cd <destination> - change the current directory based on <destination>
* ls  - Shows the names of files and folders in the current directory
* mv <src> <dst> - move a file or directory from <src> to <dst>
* cp <src> <dst>  - copy a file at <src> to a destination at <dst>
* mkdir <name> - will create a directory with the filename specified by <name>
* rm <filename> - only works for removing files (not directories). will remove the file specified at destination <filename>
* cat <filename> - displays the contents of a a file at destination <filename> to the console

The program is also capable of reading commands from a batch file whose name is specified in
the command line via file mode. To run the shell in file mode, type: ./pseudo-shell -f
\<filename\>, where \<filename\> is the name of the batch file we wish to read commands from.
The input file will contain lines of text, where each line represents either a single command to be
executed or a sequence of commands to be executed. When we use this shell in file mode, it will
generate a file “output.txt”. The output file “output.txt” will have all the same STDOUT as
running in interactive mode for the given commands
