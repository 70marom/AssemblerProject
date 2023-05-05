#include "symbols_table.h"
#include "data_handler.h"
#include "commands_handler.h"
#include "utils.h"

int IC;

bool firstPass(char *fileName)
{
    int i;
    bool errorFlag = false;

    char line[LINE_LENGTH];
    char zeroString[LINE_LENGTH] = {0};
    char *linePtr;
    char *token;
    char *numData;
    char *structData;
    char *paramPtr;
    char label[LABEL_LENGTH];
    char sourceParam[LINE_LENGTH], destParam[LINE_LENGTH];
    FILE *srcFile;
    const char delim[] = " \t";
    commandWord command;
    memoryWord memWord;
    parameters params;
    typeOfAddressing addressDest = errorAdressing;
    typeOfAddressing addressSource = errorAdressing;
    if (!(srcFile = fopen(fileName, "r"))) /* open the current file only to read it */
    {
        printf("Failed to open the file \"%s\"!\n", fileName);
        return false;
    }
    IC = START_ADDRESS;
    lineCounter = 0;
    for (i = 0; i < LINE_LENGTH; i++)
        line[i] = 0; /* reseting the array */
    while (fgets(line, LINE_LENGTH, srcFile) != NULL)
    {
        memset(label, 0, sizeof(label));
        memset(sourceParam, 0, sizeof(sourceParam));
        memset(destParam, 0, sizeof(destParam));
        resetWord(&memWord);
        lineCounter++;

        if (strlen(line) >= LINE_LENGTH)
        {
            fprintf(stderr, "In line %d there are too many characters in the line\n", lineCounter);
            errorFlag = true;
        }

        linePtr = line;

        while (linePtr[0] == ' ' || linePtr[0] == '\t')
            linePtr++; /* getting to the first letter */

        if (linePtr[strlen(linePtr) - 1] == '\n')
            linePtr[strlen(linePtr) - 1] = 0;

        if (isEmpty(linePtr) || linePtr[0] == ';')
            continue;

        token = (char *)malloc(LINE_LENGTH * sizeof(char));
        if (!token)
        {
            fprintf(stderr, "Failed to allocate memory for the token in the first pass\n");
            exit(0);
        }
        token = strtok(linePtr, delim);

        if (token[strlen(token) - 1] == ':') /* if it's a label */
        {
            if (strlen(token) > LABEL_LENGTH)
            {
                fprintf(stderr, "In line %d the label is too long (more than 30 chars)\n", lineCounter);
                errorFlag = true;
            }
            if (token[0] >= '0' && token[0] <= '9')
            {
                fprintf(stderr, "In line %d the label can't start with a digit\n", lineCounter);
                errorFlag = true;
            }
            strncpy(label, token, strlen(token) - 1);
            label[strlen(token) - 1] = 0; /* null terminated */
            if (isSavedWord(label))       /* a label can't be a saved word */
            {
                fprintf(stderr, "In line %d the label can't be a saved word\n", lineCounter);
                errorFlag = true;
            }
            token = strtok(NULL, delim);
            if (token == NULL || isEmpty(token) || token[0] == '\n') /* if there's no information / commands after the label declaration */
            {
                fprintf(stderr, "In line %d no commands after the label\n", lineCounter);
                errorFlag = true;
            }
        }

        if (token != NULL && token[0] == '.')
        {
            if (!strcmp(token, ".struct"))
            {
                if (label[0] != 0 && !isEmpty(label)) /* if there's a label before */
                {
                    if (!addToSymbolsList(label, DCgetter(), false, true, false, false)) /* add it to the symbols table */
                    {
                        fprintf(stderr, "In line %d failed to add the label to the symbols list\n", lineCounter);
                        errorFlag = true;
                    }
                }
                structData = strtok(NULL, ",");
                if (addNumToData(structData)) /* add the integer to the data image */
                {
                    structData = strtok(NULL, "\n\0");
                    structData = jumpFromLeftAndRight(structData);
                    if (!addStringToData(structData)) /* add the string to the data image */
                        fprintf(stderr, "In line %d failed to store the string in the struct\n", lineCounter);
                }
                else
                {
                    fprintf(stderr, "In line %d failed to store the number in the struct\n", lineCounter);
                    errorFlag = true;
                    break;
                }
            }
            else if (!strcmp(token, ".data")) /* done */
            {
                if (label[0] != 0 && !isEmpty(label)) /* if there's a label before */
                {
                    if (!addToSymbolsList(label, DCgetter(), false, true, false, false)) /* add it to the symbols table */
                    {
                        fprintf(stderr, "In line %d failed to add the label to the symbols list\n", lineCounter);
                        errorFlag = true;
                    }
                }
                numData = strtok(NULL, ","); /* get the first number */
                if (numData == NULL)
                {
                    fprintf(stderr, "In line %d there are missing arguments for the .data request\n", lineCounter);
                    errorFlag = true;
                }

                while (numData) /* while there are still numbers after the .data */
                {
                    if (addNumToData(numData)) /* add the current number to the data */
                        numData = strtok(NULL, ",");
                    else
                    {
                        fprintf(stderr, "In line %d failed to store the numbers in the memory\n", lineCounter);
                        errorFlag = true;
                        break;
                    }
                }
            }
            else if (!strcmp(token, ".string")) /* done */
            {
                if (label[0] != 0 && !isEmpty(label)) /* if there's a label before */
                {
                    if (!addToSymbolsList(label, DCgetter(), false, true, false, false)) /* add it to the symbols table */
                    {
                        fprintf(stderr, "In line %d failed to add the label to the symbols list\n", lineCounter);
                        errorFlag = true;
                    }
                }
                token = strtok(NULL, "\n\0"); /* get the whole string after the .string */
                token = jumpFromLeftAndRight(token);
                if (!addStringToData(token)) /* add the string to the data */
                {
                    fprintf(stderr, "In line %d failed to store the string in the memory\n", lineCounter);
                }
            }
            else if (!strcmp(token, ".extern"))
            {
                token = strtok(NULL, delim);                                /* get the whole label name */
                if (!addToSymbolsList(token, 0, false, false, false, true)) /* add it to the symbols table as an external label */
                {
                    fprintf(stderr, "In line %d failed to add the label to the symbols list\n", lineCounter);
                    errorFlag = true;
                }
            }
            else if (!strcmp(token, ".entry"))
            {
            } /* handle .entry commands in the second pass */
            else
            {
                fprintf(stderr, "In line %d there's an unknown request after the dot\n", lineCounter);
                errorFlag = true;
            }
        }
        else /* should be a command */
        {
            if (!isEmpty(label))
                errorFlag = !addToSymbolsList(label, IC, true, false, false, false);
            command = getCommandName(token); /* get all the info about the command */
            if (command.op == opError)       /* illegal command */
            {
                fprintf(stderr, "In line %d there's an unknown command\n", lineCounter);
                errorFlag = true;
            }
            memWord.opcode = command.op;

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
                memWord.source = 0;
                memWord.dest = 0;
                createImgPart(memWord, IC); /* add command to the memory image */
                IC++;
            }
            else if (command.numOfParameters == 1)
            {
                if (!strcmp(params.firstParameter, zeroString) || strcmp(params.secondParameter, zeroString)) /* there's a second parameter (illegal) */
                {
                    fprintf(stderr, "In line %d the command need to have only one parameter\n", lineCounter);
                    errorFlag = false;
                }
                memWord.source = 0;

                strcpy(destParam, params.firstParameter);
                addressDest = getAddressingType(destParam); /* get the addressing type of the first parameter */
                memWord.dest = addressDest;
                createImgPart(memWord, IC); /* add command to the memory image */
                if (addressDest == errorAdressing)
                {
                    fprintf(stderr, "In line %d the addressing type is invalid\n", lineCounter);
                    errorFlag = true;
                }
                else
                {
                    if (addressDest == registerAddressing || addressDest == immediate) /* struct and direct will be handled in the second pass, not enough info right now */
                        if (addToImg(destParam, addressDest, IC + 1, false))
                            errorFlag = true;
                    if (addressDest == structAddressing)
                        IC++;
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
                memWord.source = addressSource;

                paramPtr = params.secondParameter;
                strcpy(destParam, paramPtr);
                addressDest = getAddressingType(destParam); /* get the addressing type of the second parameter */
                memWord.dest = addressDest;

                createImgPart(memWord, IC); /* add command to the memory image */

                if (addressSource == errorAdressing || addressDest == errorAdressing)
                {
                    fprintf(stderr, "In line %d the addressing type is invalid\n", lineCounter);
                    errorFlag = true;
                }
                /* if both parameters are registers we only need 1 word for them, not 2 nor 3 */
                else if (addressDest == registerAddressing && addressSource == registerAddressing)
                {
                    addTwoRegistersToImg(sourceParam, destParam, IC + 1); /* add the 2 registers to the same word */
                    IC += 2;                                              /* we need only 1 more word, not 2+ */
                    continue;
                }
                else
                {
                    if (addressSource != errorAdressing)
                    {
                        if (addressSource == registerAddressing || addressSource == immediate) /* struct and direct will be handled in the second pass, not enough info right now */
                            if (addToImg(sourceParam, addressSource, IC + 1, true))
                                errorFlag = true;
                        /* struct addressing require an additional word, for the field of the struct */
                        if (addressSource == structAddressing)
                            IC++;
                    }
                    if (addressDest != errorAdressing)
                    {
                        if (addressDest == registerAddressing || addressDest == immediate) /* struct and direct will be handled in the second pass, not enough info right now */
                            if (addToImg(destParam, addressDest, IC + 2, false))
                                errorFlag = true;
                        /* struct addressing require an additional word, for the field of the struct */
                        if (addressDest == structAddressing)
                            IC++;
                    }
                }
                IC += 3;
            }
        }
    }
    return !errorFlag;
}