#ifndef BIT_AES_MAIN_H
#define BIT_AES_MAIN_H

#define MAIN_WRONG_PARAM_COUNT "EXITING - WRONG PARAMETERS COUNT... PLEASE ENTER KEY + SOURCE FILE + TARGET FILE (or - for console output).\n"
#define MAIN_WRONG_SEC_PAR "Second parameter not valid! Please use key which will have valid length, that´s 1 - 32 characters for 128-bit AES!\n"
#define MAIN_WRONG_THIRD_PAR "Third parameter not valid! Please enter text which you want to encrypt in between \" or path to file, which you want to encrypt.\n"
#define MAIN_WRONG_FOURTH_PAR "Fourth parameter not valid! Please enter target file, to which result will be saved or enter \"-\" for console output.\n"

/* ____________________________________________________________________________

    Function prototypes
   ____________________________________________________________________________
*/
int check_args_count(int argc);
int check_args_validity(char *argv[]);

#endif //BIT_AES_MAIN_H
