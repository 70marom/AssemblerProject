#include "global.h"
#include "spread_macros.h"
#include "utils.h"

macPtr head;

bool mainMacrosProcess(char *fileName)
{
    FILE *output;
    FILE *srcFile;
    bool isMacro = false;
    macPtr curr = NULL;
    macPtr inside = NULL;
    int i, contentCnt;
    char line[LINE_LENGTH];
    char buffer[LINE_LENGTH];
    char *linePtr;
    char *token;
    const char delim[] = " \t\n";
    if (!(srcFile = fopen(joinStrings(fileName, ".as"), "r"))) /* opening the .as file only to read it */
    {
        printf("open file failed\n");
    }
    if (!(output = fopen(joinStrings(fileName, ".am"), "w"))) /* creating a new file with the ending of .am (the .as file after the macros process) */
    {
        printf("open file failed\n");
    }
    createMacList(); /* create the linked list */

    while (fgets(line, LINE_LENGTH, srcFile) != NULL)
    {
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = 0; /* null terminated */
        strcpy(buffer, line);
        if (isEmpty(line)) /* if the current line is empty we just copy it to the file and continue */
        {
            fputs(buffer, output);
            continue;
        }
        linePtr = line;
        token = strtok(linePtr, delim);
        if (isMacro && strcmp(token, "endmacro") != false) /* if we're inside a macro */
        {
            addContentToMacro(inside, buffer, contentCnt); /* we add the content of the current line to the macro*/
            contentCnt++;
            continue;
        }
        else if (!strcmp(token, "endmacro")) /* if it's the end of a macro */
        {
            isMacro = false; /* toggle off the flag */
            continue;
        }
        isMacro = false;
        if (inMacList(token)) /* if it's a call to spread a macro */
        {
            curr = getMacro(token);
            for (i = 0; i < curr->contentLength; i++) /* going through all the content of the macro */
            {
                fputs(curr->content[i], output);
                fputs("\n", output);
            }
            continue;
        }
        if (!strcmp(token, "macro")) /* initializing a macro */
        {
            isMacro = true;
            contentCnt = 0;
            token = strtok(NULL, delim); /* getting the name of the macro */
            if (isSavedWord(token))      /* the name of the macro can't be a saved word of the language */
            {
                fprintf(stderr, "The macro: %s is a saved word for the language, can't continue the process\n", token);
                return false;
            }
            addMacro(token); /* add it to the list */
            inside = getMacro(token);
            continue;
        }
        fputs(buffer, output);
    }
    /* in the end we need to free the allocated memory and close the files */
    freeMacrosList();
    fclose(srcFile);
    fclose(output);
    return true;
}

void createMacList()
{
    /* create one node, the head, of the linked list */
    head = (macPtr)malloc(sizeof(macNode));
    if (!head)
    {
        fprintf(stderr, "Allocation memory to create macros linked list failed\n");
        exit(0);
    }
    head->name = "";
    head->content = NULL;
    head->contentLength = 0;
    head->next = NULL;
}

bool inMacList(char *str)
{
    macPtr temp = head;
    if (temp == NULL)
        return false;
    while (temp != NULL) /* going through all the list */
    {
        if (!strcmp(str, temp->name))
            return true; /* if we found a macro with the same name, return true */
        temp = temp->next;
    }
    return false; /* otherwise, return falase */
}

void addMacro(char *newName)
{
    macPtr new, temp = head;
    new = (macPtr)malloc(sizeof(macNode)); /* allocate memory for the new node of the macro */
    if (!new)
    {
        fprintf(stderr, "Memory allocation for a new macro failed!\n");
        exit(0);
    }
    while (temp->next != NULL) /* getting to the end of the list */
        temp = temp->next;
    temp->next = new;

    new->name = (char *)malloc(sizeof(char) * strlen(newName));
    if (!new->name)
    {
        fprintf(stderr, "Memory allocation for a new macro name failed!\n");
        exit(0);
    }
    strcpy(new->name, newName);

    new->next = NULL;

    new->content = malloc(sizeof(char *));
    if (!new->content)
    {
        fprintf(stderr, "Memory allocation for new macro content failed!\n");
        exit(0);
    }

    new->contentLength = 0; /* no content in it currently */
}

macPtr getMacro(char *macName)
{
    macPtr temp = head;

    while (temp != NULL)
    {
        if (!strcmp(macName, temp->name))
            return temp; /* if we found a macro with the same name in the list, return it */
        temp = temp->next;
    }
    return head;
}

void addContentToMacro(macPtr macro, char *content, int place)
{
    /* allocate enough memory using the length of the new content */
    macro->content[place] = (char *)malloc(sizeof(char) * strlen(content));
    if (!macro->content[place])
    {
        fprintf(stderr, "Memory allocation for adding content to a macro failed!\n");
        exit(0);
    }
    if (content[strlen(content) - 1] == '\n')
        content[strlen(content) - 1] = 0; /* null terminated */
    strcpy(macro->content[place], content);
    macro->contentLength++;
}

void freeMacrosList()
{
    macPtr temp;
    while (head != NULL) /* going through the list and free each node in it */
    {
        temp = head;
        head = head->next;
        free(temp);
    }
}