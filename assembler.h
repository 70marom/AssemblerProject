/* Start the assembler process on 1 file, the function returns nothing
    fileName is the name of the file to start processing (string) */
void startProcess(char *);
/* Start the process of macros spreading on 1 file, the function returns nothing
    file name is the name of the file to process */
bool mainMacrosProcess(char *);
/* The main function of the first pass process, the function process 1 file and returns true if it found errors in the file,
    it'll return false if everything went as expected. */
bool firstPass(char *);
/* The main function of the second pass process, the function process 1 file and returns true if it found errors during the second pass,
    it'll return false if everything went as expected. The int is for the IC. */
bool secondPass(char *, int);