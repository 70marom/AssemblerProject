#include "global.h"
#include "utils.h"
#include "commands_handler.h"
#include "files_handler.h"

bool secondPass(char *fileName, int IC)
{
    char line[LINE_LENGTH];
    char *linePtr;
    char *token;
    char *paramPtr;
    char sourceParam[LINE_LENGTH], destParam[LINE_LENGTH];
    char zeroString[LINE_LENGTH] = {0};
    const char delim[] = " \t";

    FILE *srcFile;
    bool errorFlag = false;

    symbol *sym;
    parameters params;
    typeOfAddressing addressDest = errorAdressing;
    typeOfAddressing addressSource = errorAdressing;
    commandWord command;

    lineCounter = 0;

    if (!(srcFile = fopen(fileName, "r"))) /* open the current file only to read it */
    {
        printf("Failed to open the file \"%s\"!\n", fileName);
        return false;
    }
    while (fgets(line, LINE_LENGTH, srcFile) != NULL)
    {
        lineCounter++;
        memset(sourceParam, 0, sizeof(sourceParam));
        memset(destParam, 0, sizeof(destParam));

        linePtr = line;

        while (linePtr[0] == ' ' || linePtr[0] == '\t')
            linePtr++; /* getting to the first letter */

        if (linePtr[strlen(linePtr) - 1] == '\n')
            linePtr[strlen(linePtr) - 1] = 0; /* null terminated */

        if (isEmpty(linePtr) || linePtr[0] == ';')
            continue;

        token = (char *)malloc(LINE_LENGTH * sizeof(char));
        if (!token)
        {
            fprintf(stderr, "Failed to allocate memory for the token in the second pass\n");
            exit(0);
        }
        token = strtok(linePtr, delim);

        if (token[strlen(token) - 1] == ':')
            token = strtok(NULL, delim);

        if (token[0] == '.') /* it might be a .entry requrst */
        {
            if (!strcmp(token, ".entry")) /* if it it .entry */
            {
                token = strtok(NULL, delim); /* get the label */
                sym = getSymbol(token);      /* get the symbol with the same label as requested */

                if (!sym) /* if it doesn't exist */
                {
                    fprintf(stderr, "In line %d the label %s wasn't declared\n", lineCounter, token);
                    errorFlag = true;
                    continue;
                }
                else
                {
                    if (sym->encode != external)                /* make sure this symbol isn't external */
                        writeToEntry(sym->label, sym->address); /* if it's not, we write it to the entry file just as requested */
                    else
                        fprintf(stderr, "In line %d the label %s is external, you can't use it for entry\n", lineCounter, token);
                }
            }
        }
        else
        {
            command = getCommandName(token);
            if (command.op == opError) /* illegal command */
            {
                fprintf(stderr, "In line %d the command is invalid\n", lineCounter);
                errorFlag = true;
                continue;
            }

            token = strtok(NULL, "");

            if (token != NULL)
                params = parametersGetter(token); /* get all the parameters after the command */
            else
            {
                memset(params.firstParameter, 0, LINE_LENGTH);
                memset(params.secondParameter, 0, LINE_LENGTH);
            }

            if (command.numOfParameters == 0)
            {
                if (strcmp(params.firstParameter, zeroString) || strcmp(params.secondParameter, zeroString)) /* there's an illegal parameter */
                {
                    fprintf(stderr, "In line %d the command %s shouldn't get any parameters\n", lineCounter, command.commandName);
                    errorFlag = true;
                }
                /* we already handeled the command in the first pass, there's nothing to do now in the second pass */
                IC++;
            }
            else if (command.numOfParameters == 1)
            {
                if (!strcmp(params.firstParameter, zeroString) || strcmp(params.secondParameter, zeroString)) /* there's a second parameter (illegal) */
                {
                    fprintf(stderr, "In line %d the command need to have only one parameter\n", lineCounter);
                    errorFlag = false;
                }
                strcpy(destParam, params.firstParameter);
                addressDest = getAddressingType(destParam); /* get the addressing type of the first parameter */
                if (addressDest == errorAdressing)
                {
                    fprintf(stderr, "In line %d the addressing type is invalid\n", lineCounter);
                    errorFlag = true;
                }
                else
                {
                    /* now we got the info to handle direct and struct addressing */
                    if (addressDest == direct || addressDest == structAddressing)
                    {
                        if (addToImg(destParam, addressDest, IC + 1, false))
                            errorFlag = true;
                        if (addressDest == structAddressing)
                            IC++;
                    }
                }
                IC += 2;
            }
            else if (command.numOfParameters == 2)
            {
                if (!strcmp(params.firstParameter, zeroString) || !strcmp(params.secondParameter, zeroString)) /* check if there are 2 parameters */
                {
                    fprintf(stderr, "In line %d the command need to have two parameters\n", lineCounter);
                    errorFlag = true;
                }

                paramPtr = params.firstParameter;
                strcpy(sourceParam, paramPtr);
                addressSource = getAddressingType(sourceParam); /* get the addressing type of the first parameter */

                paramPtr = params.secondParameter;
                strcpy(destParam, paramPtr);
                addressDest = getAddressingType(destParam); /* get the addressing type of the second parameter */

                if (addressSource == errorAdressing || addressDest == errorAdressing)
                {
                    fprintf(stderr, "In line %d the addressing type is invalid\n", lineCounter);
                    errorFlag = true;
                }
                /* if both parameters are registers we only need 1 word for them, not 2 nor 3 */
                else if (addressDest == registerAddressing && addressSource == registerAddressing)
                {
                    IC += 2;
                    continue;
                }
                else
                {
                    if (addressSource != errorAdressing)
                    {
                        /* we already handled register and immediate addressing, now we handle direct and struct */
                        if (addressSource == direct || addressSource == structAddressing)
                        {
                            if (addToImg(sourceParam, addressSource, IC + 1, false))
                                errorFlag = true;
                            /* struct addressing require an additional word, for the field of the struct */
                            if (addressSource == structAddressing)
                                IC++;
                        }
                    }
                    if (addressDest != errorAdressing)
                    {
                        /* we already handled register and immediate addressing, now we handle direct and struct */
                        if (addressDest == direct || addressDest == structAddressing)
                        {
                            if (addToImg(destParam, addressDest, IC + 2, true))
                                errorFlag = true;
                            /* struct addressing require an additional word, for the field of the struct */
                            if (addressDest == structAddressing)
                                IC++;
                        }
                    }
                }
                IC += 3;
            }
        }
    }
    return !errorFlag;
}