#ifndef SYMBOLS_TABLE_H
#define SYMBOLS_TABLE_H

#include "global.h"

/* Linked list for the symbols (labels) */
typedef struct node *symbolNodePtr;
typedef struct node
{
    char *label;
    int address;
    bool isData;
    bool isExtern;
    bool isComm;
    bool isEntry;
    symbolNodePtr next;
} symbolNode;

/* Create the head of the linked list of the symbols table list (it'll have no data in it) */
void createList();
/* Add a new label to the symbols list, get it's name, address, if it's a command, data, entry or external - also check if there's
    already a symbol with the same name in the list */
bool addToSymbolsList(char *, int, bool, bool, bool, bool);
/* The function gets a label and returns if there's already a symbol with the same label in the linked list, if there is return true */
bool inSymbolList(char *);
/* The function gets a label and returns info about that label in the special symbol struct we made (if there is a symbol with the same label)
    the struct will have the label, the address and the ARE mode of the given symbol */
symbol *getSymbol(char *);
/* The function gets an integer and goes through the linked list of the symbols, it'll add to each symbol's address
    (if it's not a command or external) the number the function got */
void UpdateSymbolTable(int);
/* The function goes through the linked list and free each node because of the allocation memory for the nodes */
void freeSymbolsTableNodes();

#endif