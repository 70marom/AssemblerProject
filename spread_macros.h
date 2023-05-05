#ifndef SPREAD_MACROS_H
#define SPREAD_MACROS_H

/* We use a linked list to store our macros, name is the name of the macros
    content is the content of the macro (could more than 1 line) and contentLength is the number of lines of the content */
typedef struct macList *macPtr;
typedef struct macList
{
    char *name;
    char **content;
    int contentLength;
    macPtr next;
} macNode;

/* Create the head of the linked list, it'll have no content (for now) and the head will point to NULL */
void createMacList();
/* The function is getting a string and going through the linked list to check if there's a macro with the same name of the given true,
    return true is there is one, false - if there's no macro with the same name */
bool inMacList(char *);
/* Add a new macro to the end of the linked list, currently it'll have no content in it, we only add it to the list */
void addMacro(char *);
/* Search the list for a macro with the same name as the given string and return a pointer to it */
macPtr getMacro(char *);
/* Add a new line of content to a specific macro, get a pointer to the specific macro, the line to add (a string) and the index
    to know where to place the new content (what line to place it) */
void addContentToMacro(macPtr, char *, int);
/* Free every node in the macros linked list (because we allocated memory for each node) */
void freeMacrosList();

#endif