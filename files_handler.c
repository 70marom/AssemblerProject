#include "files_handler.h"
#include "utils.h"

FILE *entryFile;
FILE *externFile;
FILE *objectFile;

void writeToEntry(char *label, int address)
{
    /* if the entry file is unopened and we failed to create it (a new file with the extension .ent) */
    if (!entryFile && !(entryFile = fopen(joinStrings(fileName, ".ent"), "w+")))
        fprintf(stderr, "Failed to create an entry file\n");
    else
        fprintf(entryFile, "%s\t%s\n", label, baseConvertTo32(address)); /* write the data to the entry file, label and it's address in the unique base */
}

void writeToExternal(char *label, int address)
{
    /* if the extern file is unopened and we failed to create it (a new file with the extension .ext) */
    if (!externFile && !(externFile = fopen(joinStrings(fileName, ".ext"), "w+")))
        fprintf(stderr, "Failed to create an external file\n");
    else
        fprintf(externFile, "%s\t%s\n", label, baseConvertTo32(address)); /* write the data to the extern file, label and it's address in the unique base */
}

void writeToObject(int address, int code)
{
    /* if the object file is unopened and we failed to create it (a new file with the extension .obj) */
    if (!objectFile && !(objectFile = fopen(joinStrings(fileName, ".ob"), "w+")))
        fprintf(stderr, "Failed to create an object file\n");
    else
    {
        if (!ftell(objectFile)) /* if it's the start of the file we need to create a header */
        {
            fprintf(objectFile, "    %s ", baseConvertTo32(address - START_ADDRESS)); /* the number of used used words for commands */
            fprintf(objectFile, "%s\n", baseConvertTo32(code));                       /* the number of used used words for data */
        }
        else
            fprintf(objectFile, "%s\t%s\n", baseConvertTo32(address), baseConvertTo32(code));
    }
}

void resetFiles()
{
    entryFile = externFile = objectFile = NULL;
}