#include "data_handler.h"
#include "utils.h"

int DC = 0;
int DATA[DATA_SIZE]; /* The data will have 256 free places */

bool addNumToData(char *num)
{
    bool errorFlag = false;
    if (DC < DATA_SIZE) /* there's free memory to store the number */
    {
        int saveNum;
        char *endptr;
        saveNum = strtol(num, &endptr, 10); /* parsing string to int in base 10 */
        if (saveNum <= MAX_NUM && saveNum >= MIN_NUM)
        {
            if ((endptr != NULL && endptr != num && isEmpty(endptr)) || endptr == NULL) /* there should not be any character after the number */
            {
                /* we store the number and adding 1 to the counter */
                DATA[DC] = saveNum;
                DC++;
            }
            else /* if there's something wrong */
            {
                errorFlag = true;
                fprintf(stderr, "Failed to store: %s\n", num);
            }
        }
        else /* if the number is out of our range (which is -512 - 511) */
        {
            fprintf(stderr, "In line %d the number %d is out of range (not enough memory to save it)\n", lineCounter, saveNum);
            errorFlag = true;
        }
    }
    else /* if there's not enough memory to store the number */
    {
        fprintf(stderr, "In line %d there's no more memory to store new data\n", lineCounter);
        errorFlag = true;
    }

    return !errorFlag;
}

bool addStringToData(char *string)
{
    bool errorFlag = false;
    int lastPos = strlen(string) - 1; /* the last index of the given string */
    if (string[lastPos] == '\n')
        string[lastPos] = 0;
    if (string[0] == '\"' && string[lastPos] == '\"') /* the syntax of strings in the assembly */
    {
        for (string++; string[0] != '\"' && DC < DATA_SIZE; DC++, string++) /* going through each character of the string and storing it */
            DATA[DC] = string[0];
        DATA[DC] = 0; /* null terminated, in the end of the string */
        DC++;

        if (DC >= DATA_SIZE) /* after storing the string, check if there's free memory */
        {
            fprintf(stderr, "In line %d there's no more memory to store new data\n", lineCounter);
            errorFlag = true;
        }
    }
    else /* if the syntax isn't okay */
    {
        fprintf(stderr, "Expected a string, %s isn't a string\n", string);
        errorFlag = true;
    }

    return !errorFlag;
}

int DCgetter() { return DC; }
void DCreset() { DC = 0; }

void writeDataInObject(int start)
{
    int i;
    for (i = start; i < start + DC; ++i)
        writeToObject(i, DATA[i - start]);
}