#include "global.h"
#include "utils.h"

bool fileExist(char *fileName)
{
    FILE *ftmp;
    if (!(ftmp = fopen(fileName, "r"))) /* if we failed to open the file, it doesn't exist */
        return false;
    fclose(ftmp);
    return true;
}

bool isComment(char *str)
{
    char *tmp = jumpFromLeft(str); /* get to the first letter from the left */
    if (tmp[0] == ';')
        return true;
    return false;
}
bool isEmpty(char *str)
{
    int i = 0;
    bool empty = true;
    if (strcmp(str, "")) /* if there're characters in the given string */
    {
        while (empty && str && str[i] != '\0' && str[i] != '\n') /* while it's not the end of the string */
        {
            if (isspace(str[i]) || str[i] == '\t') /* it's an "empty" char, we can continue*/
                i++;
            else /* if it's not, the string isn't empty */
                empty = false;
        }
    }
    return empty;
}

char *jumpFromLeft(char *str)
{
    if (str == NULL)
        return NULL;
    /* while we are on empty characters */
    while (isspace((unsigned char)*str))
        str++;
    return str;
}

char *jumpFromLeftAndRight(char *str)
{
    char *end;
    if (str == NULL)
        return NULL;
    /* while we are on empty characters */
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
    return str;
}

char *joinStrings(const char *str1, const char *str2)
{
    char *result = (char *)malloc(strlen(str1) + strlen(str2) + 1);
    if (!result)
        fprintf(stderr, "Memory allocation to join 2 strings failed!\n");
    strcpy(result, str1); /* first we copy the first string */
    strcat(result, str2); /* and then the second one, and we get a new string with both of them */
    return result;
}

char *spacesRemoval(char *str)
{
    char *temp = str;
    int i;
    int count = 0;
    for (i = 0; temp[i]; i++)
    {
        if (temp[i] != ' ' && temp[i] != '\t' && temp[i] != '\n') /* if it's not an empty char */
            temp[count++] = temp[i];
    }
    /* null terminated */
    temp[count] = 0;
    temp[count + 1] = 0;
    return temp;
}

bool isSavedWord(char *check)
{
    char *savedWords[] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "get", "prn", "jsr", "rts", "hlt",
                          ".data", ".extern", ".entry", ".string", ".struct", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int i = 0;
    for (i = 0; i < SAVED_WORD_AMMOUNT; i++) /* going through all the 29 saved words of the language */
    {
        if (!strcmp(check, savedWords[i])) /* if the given string is a saved word we return true */
            return true;
    }
    return false;
}

bool isRegister(char *check)
{
    int i;
    char *registers[REGISTERS_AMMOUNT] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    if (strlen(check) == 2) /* the length has to be 2 */
    {
        for (i = 0; i < REGISTERS_AMMOUNT; i++) /* going through all the 8 registers */
        {
            if (!strcmp(check, registers[i])) /* if the given string is a register we return true */
                return true;
        }
    }
    return false;
}

char *baseConvertTo32(int toConvert)
{
    char *uniqueBase[33] = {"!", "@", "#", "$", "%", "^", "&", "*", "<", ">", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v"};
    char *firstDigit, *secondDigit, *result;
    result = (char *)malloc(sizeof(char) * 3); /* we only need 3 character for the result */
    if (result == NULL)
    {
        fprintf(stderr, "Memory allocation for base convertion failed\n");
        exit(0);
    }
    if (toConvert < 0) /* if the number we convert is negative */
        toConvert = MAX_NUM_IN_MEMORY - (toConvert * (-1));
    /* we do the base convertion, using the converted number (in base 32) as an index for the digit in the special base (which is also base 32) */
    firstDigit = uniqueBase[toConvert % 32];
    toConvert = toConvert / 32;
    secondDigit = uniqueBase[toConvert % 32];
    result = joinStrings(secondDigit, firstDigit); /* join the 2 digits toghether and store it in the result */
    return result;
}

bool isStruct(char *toCheck)
{
    char *token, *temp;
    int num;
    int checkLength = strlen(toCheck);
    temp = (char *)malloc(sizeof(char) * (checkLength + 1));
    if (temp == NULL)
    {
        fprintf(stderr, "Memory allocation for isStruct method failed\n");
        exit(0);
    }
    strncpy(temp, toCheck, checkLength + 1);

    token = strtok(temp, ".");
    token = strtok(NULL, ".");

    if (token == NULL) /* if there's nothing after the dot, it's not a struct */
    {
        free(temp);
        return false;
    }
    spacesRemoval(token);
    num = atoi(token);        /* get the field of the struct */
    if (num != 2 && num != 1) /* the field can be only 1 or 2 */
    {
        free(temp);
        return false;
    }
    token = strtok(NULL, ".");
    free(temp);
    if (token == NULL) /* if there's nothing else after the field, it's a struct */
        return true;
    else
        return false;
}