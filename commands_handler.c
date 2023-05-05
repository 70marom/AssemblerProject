#include "commands_handler.h"
#include "utils.h"

commandWord table[COMMANDS_AMOUNT] =
    {
        {"mov", mov, 2},
        {"cmp", cmp, 2},
        {"add", add, 2},
        {"sub", sub, 2},
        {"not", not, 1},
        {"clr", clr, 1},
        {"lea", lea, 2},
        {"inc", inc, 1},
        {"dec", dec, 1},
        {"jmp", jmp, 1},
        {"bne", bne, 1},
        {"get", get, 1},
        {"prn", prn, 1},
        {"jsr", jsr, 1},
        {"rts", rts, 0},
        {"hlt", hlt, 0}};

int codeImg[MAX_NUM_IN_MEMORY + START_ADDRESS]; /* 2^10 + start address (100) */

commandWord getCommandName(char *commandName)
{
    int i;
    commandWord res = {"none", opError, -1};
    for (i = 0; i < COMMANDS_AMOUNT; i++) /* going through every command */
    {
        if (!strcmp(commandName, table[i].commandName)) /* if we found a command with the same name */
        {
            res = table[i];
            return res; /* we return it */
        }
    }
    return res; /* otherwise we return a command with opcode of error */
}

void resetWord(memoryWord *word)
{
    word->encodeMethod = encodError;
    word->dest = 0;
    word->source = 0;
    word->opcode = opError;
}

parameters parametersGetter(char *line)
{
    bool errorFlag = false;
    int commaCounter = 0;
    int i = 0;
    char firstParam[LINE_LENGTH] = {0};
    char secondParam[LINE_LENGTH] = {0};
    char *linePtr;
    parameters result;

    line = spacesRemoval(line); /* remove every space from the given line */

    if (line[0] == ',') /* illegal comma check */
    {
        fprintf(stderr, "In line %d there's an illegal comma at the start\n", lineCounter);
        errorFlag = true;
    }
    linePtr = line;
    while (linePtr[0] != 0) /* counting every comma in the line */
    {
        if (linePtr[0] == ',')
            commaCounter++;
        linePtr++;
    }
    if (commaCounter > 1) /* it's illegal to have more than 1 comma (0 commas for 1 param, 1 comma for 2 params) */
    {
        fprintf(stderr, "In line %d there are too many commas\n", lineCounter);
        errorFlag = true;
    }

    if (commaCounter == 0)
    {
        i = 0;
        /* going through every char */
        while (line[0] != 0 && line[0] != '\n')
        {
            /* checking if the current char is legal, if so - we copy it to the array we will return (in a params struct) */
            if (isdigit(line[0]) || isalpha(line[0]) || line[0] == '#' || line[0] == '+' || line[0] == '-' || line[0] == '.')
            {
                firstParam[i] = line[0];
                line++;
                i++;
            }
            else /* illegal char */
            {
                if (line[0] != ',') /* it could be the end of the parameter, we'll check it */
                {
                    fprintf(stderr, "In line %d there's an invalid parameter: %s\n", lineCounter, line);
                    errorFlag = true;
                }
                break;
            }
        }
        /* if the process was a success we copy the parameter to the struct we made, we reset the second param */
        if (!errorFlag)
        {
            strcpy(result.firstParameter, firstParam);
            memset(result.secondParameter, 0, LINE_LENGTH);
        }
    }
    if (commaCounter == 1) /* should be 2 parameters */
    {
        i = 0;
        /* until we get to the second parameter */
        while (line[0] != ',')
        {
            /* same process as before - checking every character */
            if (isdigit(line[0]) || isalpha(line[0]) || line[0] == '#' || line[0] == '+' || line[0] == '-' || line[0] == '.')
            {
                firstParam[i] = line[0];
                line++;
                i++;
            }
            else
            {
                if (line[0] != ',')
                {
                    fprintf(stderr, "In line %d there's an invalid parameter: %s\n", lineCounter, line);
                    errorFlag = true;
                }
                break;
            }
        }
        line++; /* skip comma */
        if (line[0] == 0 || line[0] == '\n')
        {
            fprintf(stderr, "In line %d there's an illegal comma at the end\n", lineCounter);
            errorFlag = true;
        }

        i = 0;

        /* now we check and copy the second parameter until the end (null or newline) */
        while (line[0] != 0 && line[0] != '\n')
        {
            if (isdigit(line[0]) || isalpha(line[0]) || line[0] == '#' || line[0] == '+' || line[0] == '-' || line[0] == '.')
            {
                secondParam[i] = line[0];
                line++;
                i++;
            }
            else
            {
                if (line[0] != 0 && line[0] != '\n')
                {
                    fprintf(stderr, "In line %d there's an invalid parameter - %s\n", lineCounter, line);
                    errorFlag = true;
                }
                break;
            }
        }
        /* if the process was a success we copy the parameters to the struct we made */
        if (!errorFlag)
        {
            strcpy(result.firstParameter, firstParam);
            strcpy(result.secondParameter, secondParam);
        }
    }
    /* if something went wrong in the process the we reset the 2 params (inside the result struct) */
    if (errorFlag)
    {
        memset(result.firstParameter, 0, LINE_LENGTH);
        memset(result.secondParameter, 0, LINE_LENGTH);
    }
    return result;
}

void createImgPart(memoryWord memWord, int IC)
{
    codeImg[IC] = memWord.opcode;
    codeImg[IC] = (codeImg[IC] << 2) + memWord.source; /* type of source addressing */
    codeImg[IC] = (codeImg[IC] << 2) + memWord.dest;   /* type of destination addressing */
    codeImg[IC] = (codeImg[IC] << 2);
}

typeOfAddressing getAddressingType(char *parameter)
{
    typeOfAddressing res = errorAdressing;
    char label[LABEL_LENGTH] = {0};
    char ch[1] = {0};
    int i;
    char *temp;
    char *token;
    int num = MAX_NUM;
    char tempParam[LINE_LENGTH] = {0};
    token = (char *)malloc(LINE_LENGTH * sizeof(char));
    if (!token)
    {
        fprintf(stderr, "Memory allocation to get the type of addressing failed\n");
        exit(0);
    }
    if (isRegister(parameter))
        res = registerAddressing;
    else if (isStruct(parameter))
        res = structAddressing;
    /* could be immediate or direct addressing */
    else
    {
        ch[0] = parameter[0];
        parameter++;
        /* copying the parameter to the temp one until the end of it (newline / '\0') */
        while (parameter[0] != 0 && parameter[0] != '\n')
        {
            tempParam[i] = parameter[0];
            i++;
            parameter++;
        }
        /* if it starts with '#' the adressing could be immediate */
        if (ch[0] == '#')
        {
            if (sscanf(tempParam, "%d", &num) == 1) /* getting the number*/
            {
                if (num <= MAX_NUM && num >= MIN_NUM) /* number in range */
                    res = immediate;
                else /* number out of range */
                {
                    fprintf(stderr, "In line %d the given number is out of range\n", lineCounter);
                    res = immediate;
                }
            }
        }
        else
        {
            temp = strcat(ch, tempParam);
            if (sscanf(temp, "%31[a-zA-Z0-9]", label) == 1) /* labels are only 30 chars long, with english letters and numbers */
                res = direct;
        }
    }
    free(token);
    return res;
}

bool addToImg(char *parameter, typeOfAddressing addressType, int IC, bool isDest)
{
    bool errorFlag = false;
    int num;
    symbol *symbolTemp;
    char *param = parameter;
    char *token;
    int length = strlen(param);
    int i = 0;
    char paramRes[LINE_LENGTH] = {0};

    while (i != length)
    {
        if (param[0] == '#') /* skipping the # part */
            param++;
        paramRes[i] = param[0];
        i++, param++;
    }
    if (addressType == registerAddressing)
    {
        num = paramRes[1] - '0'; /* get the number of the register */
        codeImg[IC] = num;
        if (isDest)
            codeImg[IC] = codeImg[IC] << 6; /* if it's destination we use 6-9 bits */
        else
            codeImg[IC] = codeImg[IC] << 2; /* we use 2-5 bits */
    }
    else if (addressType == immediate)
    {
        num = strtol(paramRes, NULL, 10); /* converting the string to a number */
        num <<= 2;                        /* because of ARE bits */
        codeImg[IC] = num;
    }
    else if (addressType == direct)
    {
        symbolTemp = getSymbol(parameter); /* getting the symbol to get info on the label */
        if (symbolTemp != NULL)
        {
            codeImg[IC] = symbolTemp->address;
            codeImg[IC] = (codeImg[IC] << 2) + relocatable; /* relocatable because it's a label, shifting by 2 because of ARE */
            if (symbolTemp->encode == external)
            {
                writeToExternal(symbolTemp->label, IC); /* we need to write the external label to the extern file */
                codeImg[IC] = external;
            }
        }
        else
        {
            fprintf(stderr, "In line %d the label %s is invalid\n", lineCounter, parameter);
            errorFlag = true;
        }
    }
    else if (addressType == structAddressing)
    {
        token = strtok(paramRes, "."); /* get the label of the struct */
        symbolTemp = getSymbol(token);
        if (symbolTemp != NULL)
        {
            codeImg[IC] = symbolTemp->address;              /* the address of the struct */
            codeImg[IC] = (codeImg[IC] << 2) + relocatable; /* same as before */
        }
        else
        {
            fprintf(stderr, "In line %d the symbol %s in the struct is invalid\n", lineCounter, token);
            errorFlag = true;
        }
        token = strtok(NULL, ""); /* get the field */
        num = atoi(token);
        IC += 1; /* another word for the field of the struct */
        codeImg[IC] = num << 2;
    }
    return errorFlag;
}

void addTwoRegistersToImg(char *source, char *dest, int IC)
{
    int reg1 = source[1] - '0'; /* first register number */
    int reg2 = dest[1] - '0';   /* second register number */
    /* we use the same word for the 2 register (with bits 2-5 and 6-9) */
    codeImg[IC] = reg1;
    codeImg[IC] <<= 6;
    reg2 <<= 2;
    codeImg[IC] += reg2;
}

void writeInstructionInObject(int start, int end)
{
    int i;
    for (i = start; i < end; ++i)
    {
        writeToObject(i, codeImg[i]); /* going thrrough the words in the code image and writing it to the obj file with our special 32 base */
    }
}