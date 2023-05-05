#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define LINE_LENGTH 81
#define LABEL_LENGTH 31
#define START_ADDRESS 100
#define MAX_NUM_IN_MEMORY 1024

/* one word is 10 bits so the max number is 2^9 - 1 and the min is 2^9 */
#define MAX_NUM 511
#define MIN_NUM -512

/* Dennis Ritchie didn't create booleans for us, so we create our own booleans - 0 is false, 1 is true */
typedef enum bools
{
    false,
    true
} bool;

/* The operation codes, from 0 to 15, every op code is with the name of the operation */
typedef enum ops
{
    mov,
    cmp,
    add,
    sub,
    not,
    clr,
    lea,
    inc,
    dec,
    jmp,
    bne,
    get,
    prn,
    jsr,
    rts,
    hlt,
    opError = -1
} opCode;

/* The 4 types of addressing, from 0 to 3 */
typedef enum addressing
{
    immediate,
    direct,
    structAddressing,
    registerAddressing,
    errorAdressing = -1
} typeOfAddressing;

/* As we know, absolute equals to 0 (00 in binary), external equals to 1 (01) and relocatable equals to 2 (11) */
typedef enum encoding
{
    absolute,
    external,
    relocatable,
    encodError
} ARE;

/* A struct to hold info about commands (the command's name, op code and number of operators) */
typedef struct commandStruct
{
    const char *commandName;
    opCode op;
    int numOfParameters;
} commandWord;

/* A struct to illustrate the word in the memory (ARE bits, destinition and source addressing types and op code) */
typedef struct
{
    ARE encodeMethod;
    typeOfAddressing dest;
    typeOfAddressing source;
    opCode opcode;
} memoryWord;

/* A struct with 2 string to hold 2 parameters (2 simple arrays with the length of 81) */
typedef struct parameterStruct
{
    char firstParameter[LINE_LENGTH];
    char secondParameter[LINE_LENGTH];
} parameters;

/* A struct to hold information about a specific symbol, including the label name, the address of the symbol and it's ARE type */
typedef struct symbolStruct
{
    char label[LABEL_LENGTH];
    int address;
    ARE encode;
} symbol;

/* Those 2 variables are very important for all the files, in many many functions, so we need easy access to them */
extern int lineCounter;
extern char *fileName;

#endif