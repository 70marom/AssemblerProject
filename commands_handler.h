#ifndef COMMANDS_HANDLER_H
#define COMMANDS_HANDLER_H

#include "global.h"
#include "symbols_table.h"
#include "files_handler.h"

#define COMMANDS_AMOUNT 16

/* The function return the struct of the requested command (with the name, op and number of parameters)
    if the given string isn't a command it'll return a struct with errors and "none" name */
commandWord getCommandName(char *);
/* Reset a given word, dest = source = 0, op = error, encode method = error */
void resetWord(memoryWord *);
/* Get a line of code and examine the parameters, the number of them and if they're legal.
    Return them in a special struct of parameters in the end. */
parameters parametersGetter(char *);
/* Create the memory image for the first time */
void createImgPart(memoryWord, int);
/* Return the type of addressing of a given parameter, could be a register, a struct, a label or a number.
    The function will check if the parameter is legal (like if the number is in our range of numbers) */
typeOfAddressing getAddressingType(char *);
/* Encode the information to the memory image, return true if we found errors during the process, false - otherwise */
bool addToImg(char *, typeOfAddressing, int, bool);
/* If the 2 parameters of an operation are registers, we encode them to the same word in our memory, this function will handle that case */
void addTwoRegistersToImg(char *, char *, int);
/* Going through our memory image and write every line (from start to end, with the special base) to the object file */
void writeInstructionInObject(int, int);

#endif