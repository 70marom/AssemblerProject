#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "global.h"
#include "files_handler.h"

#define DATA_SIZE 256

/* Return the Data Counter */
int DCgetter();
/* Add a number to the data memory, we get it as a string and we check if it's legal, if it is we add it to the data part,
    if not we return true and printing an error */
bool addNumToData(char *);
/* Add a string to the data memory, we get the string and we check if it's legal, if it is we add it to the data part,
    if not we return true and printing an error */
bool addStringToData(char *);
/* Going through the data memory (from start address) until the start + DC and write every information from our data memory to the obj file */
void writeDataInObject(int);
void DCreset();

#endif