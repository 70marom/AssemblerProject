#include "symbols_table.h"

symbolNodePtr head;

void createList()
{
    /* allocate memory for the head of the linked list */
    head = (symbolNodePtr)malloc(sizeof(symbolNode));
    if (!head)
    {
        fprintf(stderr, "Allocation memory to create symbol list failed\n");
        exit(0);
    }
    head->address = -1;
    head->isComm = false;
    head->isData = false;
    head->isEntry = false;
    head->isExtern = false;
    head->label = " ";
    head->next = NULL;
}

bool addToSymbolsList(char *input, int address, bool isCom, bool isDat, bool isEnt, bool isExt)
{
    bool errFlag = false;
    symbolNodePtr newNode;
    symbolNodePtr temp = head;
    newNode = (symbolNodePtr)malloc(sizeof(symbolNode));
    if (!newNode)
    {
        fprintf(stderr, "Memory allocation for a new symbol node failed\n");
        exit(0);
    }
    if (input == NULL)
        errFlag = true;
    else
    {
        if (inSymbolList(input)) /* it's illegal to have 2 labels with the same name */
        {
            fprintf(stderr, "There's a label with the name: %s already\n", input);
            errFlag = true;
        }
    }
    if (!errFlag)
    {
        /* initialize the symbol just like the parameters of the functions */
        newNode->address = address;
        newNode->isComm = isCom;
        newNode->isData = isDat;
        newNode->isEntry = isEnt;
        newNode->isExtern = isExt;
        newNode->next = NULL;

        /* allocate memory for it's name */
        newNode->label = (char *)malloc(sizeof(char) * LABEL_LENGTH);
        if (!newNode->label)
        {
            fprintf(stderr, "Memory allocation for a new node label failed\n");
            exit(0);
        }
        strcpy(newNode->label, input);
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
    return !errFlag;
}

bool inSymbolList(char *label)
{
    symbolNodePtr temp = head;
    if (temp == NULL)
        return false;
    while (temp != NULL) /* going through the list */
    {
        if (!strcmp(label, temp->label)) /* if there's a symbol with the same label as requested return true */
            return true;
        temp = temp->next;
    }
    return false; /* otherwise return falase */
}

symbol *getSymbol(char *label)
{
    symbol *result = NULL;
    symbolNodePtr temp = head;
    while (result == NULL && temp != NULL)
    {
        if (strcmp(label, temp->label)) /* continue if we didn't find the requested symbol */
            temp = temp->next;
        else
        {
            result = (symbol *)malloc(sizeof(symbol));
            if (!result)
            {
                fprintf(stderr, "Failed to allocate memory for a symbol\n");
                exit(0);
            }
            /* initialize the symbol, the result, just like the symbol in the linked list */
            strcpy(result->label, temp->label);
            result->address = temp->address;
            if (temp->isExtern)
                result->encode = external;
            else
                result->encode = relocatable;
        }
    }
    if (result == NULL)
        fprintf(stderr, "In line %d can't find the label %s\n", lineCounter, label);
    return result;
}

void UpdateSymbolTable(int toAdd)
{
    symbolNodePtr nodeTmp = head;
    while (nodeTmp != NULL)
    {
        /* only update if it's not a command or external */
        if (!nodeTmp->isComm && !nodeTmp->isExtern)
            nodeTmp->address += toAdd;
        nodeTmp = nodeTmp->next;
    }
}

void freeSymbolsTableNodes()
{
    symbolNodePtr temp;
    while (head != NULL) /* going through all the list */
    {
        temp = head;
        head = head->next;
        free(temp); /* free each node */
    }
}