/*
 * Seth Tal
 * 04.29.2020
 * Header file for MCP program
*/


/* --------------------------------------------------------------------------------------------- */
/* Function Declarations for this program */
void usage(int argc, char** argv);
void GrabInput(char* filename, char*** programList, int* size);
void printA(char** array, int length);
void ProcessInput(char** programs, int arrayLength);
void TokenizeProgram(char* toTokenize, char*** argArr, int arrLength);
/* --------------------------------------------------------------------------------------------- */