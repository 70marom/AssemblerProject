#include "global.h"
#include "symbols_table.h"
#include "data_handler.h"
#include "commands_handler.h"
#include "files_handler.h"
#include "utils.h"
#include "assembler.h"

extern int IC;

int lineCounter;
char *fileName;

int main(int argc, char *argv[])
{
    int i, lengthName;
    if (argc == 1)
    { /* there are no files */
        printf("Error: didn't recieve files from the command line\n");
    }
    for (i = 1; i < argc; i++)
    { /* running through all the files */
        lengthName = strlen(argv[i]);
        fileName = (char *)malloc((lengthName + 1) * sizeof(char));
        if (!fileName)
        {
            printf("Error: memory allocation for the file name failed\n");
            continue;
        }
        strncpy(fileName, argv[i], lengthName);
        fileName[lengthName] = '\0';
        if (!fileExist(fileName))
        { /* check if the entered file exist in the directory of the assembler */
            printf("Failed to open the file \"%s\"! Please check if you entered the file name correctly.\n", fileName);
            continue;
        }
        if (fileName[lengthName - 1] != 's' || fileName[lengthName - 2] != 'a' || fileName[lengthName - 3] != '.')
        {
            printf("The file \"%s\" isn't ending with the extension .as, only files with .as are compitable.\n", fileName);
            continue;
        }
        fileName[lengthName - 1] = fileName[lengthName - 2] = fileName[lengthName - 3] = 0;
        if (!mainMacrosProcess(fileName))
        {
            fprintf(stderr, "The process to spread the macros failed\n");
            free(fileName);
            continue;
        }
        startProcess(joinStrings(fileName, ".am")); /* after the macro process, we need to do all the operations on the .am file */
        free(fileName);
    }
    return 0;
}

void startProcess(char *fileName)
{
    createList();            /* create symbols table linked list */
    if (firstPass(fileName)) /* if first pass was a success */
    {
        UpdateSymbolTable(IC);
        if (secondPass(fileName, START_ADDRESS)) /* if second pass was a success */
        {
            writeToObject(IC, DCgetter());
            writeInstructionInObject(START_ADDRESS, IC);
            writeDataInObject(IC);
        }
        else /* second pass failed */
            fprintf(stderr, "Found error/s in the second pass!\n");
    }
    else /* first pass failed */
        fprintf(stderr, "Found error/s in the first pass!\n");
    freeSymbolsTableNodes(); /* free symbols table linked list */
    resetFiles();
    DCreset();
    fprintf(stdout, "*****\nFinished the assembly process for %s\n(Note: if there are no errors above this message, the process was a success)\n*****\n", fileName);
}