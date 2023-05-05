#define SAVED_WORD_AMMOUNT 29
#define REGISTERS_AMMOUNT 8

/* Check if the given file (the string parameter) is exists in the folder the terminal was opened from, return true is there's a file with the same name */
bool fileExist(char *);
/* Get a line of code and check if that's a comment in the assembly language, return true it is, false if it's not a comment */
bool isComment(char *);
/* Get a string and get to the first character of the string (skipping all the spaces from the left), return the new string in the end */
char *jumpFromLeft(char *);
/* The function is the same as the jumpFromLeft function but this function also skips the spaces from the right of the string, and returns it */
char *jumpFromLeftAndRight(char *str);
/* Get a string (a word) and check if it's a saved word of the assembly language, if it is - return true, if it's not - return false */
bool isSavedWord(char *);
/* Get a string and return if it's an empty string (a string with only spaces, tabs etc.), return true if it's empty */
bool isEmpty(char *);
/* The function gets 2 strings and return them joined together, for example if str1 = abc and str2 = def then it'll return "abcdef" */
char *joinStrings(const char *, const char *);
/* The function gets a string and return the same string without any spaces in it */
char *spacesRemoval(char *);
/* The function gets a string and return if it's a register of the assembly language, if it's a register return true */
bool isRegister(char *);
/* The function gets a number in decimal base and converts it to the special 32 base, it'll return the converted number as a string */
char *baseConvertTo32(int);
/* Get a string and return if it's a struct of the assembly language, for example: A.2, if it's a struct (with a field, as expected) return true, if it's not return false */
bool isStruct(char *);