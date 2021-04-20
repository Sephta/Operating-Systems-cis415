# Psuedo Shell Environmnet

## Project 1 for CIS 415 (Operating Systems) @ University of Oregon

In this project I was tasked with utilizing various Unix system calls to emulate common behavior in command line interfaces. The program was developed in C, inside of an arch linux virtual machine.

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
