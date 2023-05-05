#ifndef FILES_HANDLER_H
#define FILES_HANDLER_H

#include "global.h"

/* Create the entry file if needed and write the given label and address to the entry file with the special base */
void writeToEntry(char *, int);
/* Create the external file if needed and write the given label and address to the external file with the special base */
void writeToExternal(char *, int);
/* Create the object file if needed and write the headlines if needed (used words for data and commands),
    if it's not needed we write to the file the address and then the given code in the special base */
void writeToObject(int, int);
void resetFiles();

#endif