#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encrypt_logic.h"
#include "main.h"

char *input_content; /* plaintext to encrypt OR path to file, which user wants to encrypt */
int input = -1; /* 0 for text input (given in between '), 1 for path to file (whole file encryption) */
char *output_content; /* path to file, which will be created and will contain encrypted data */
int output = -1; /* 0 for output to console, 1 for output to file */
unsigned char *key_to_use; /* key to use for encrypting / decrypting */

/* ____________________________________________________________________________
    int main(int argc, char *argv[])

    Starting point of the program. Arguments specified by argv are:
    0) name of the program
    1) key, with which file or plaintext message will be encrypted
    2) Plaintext, which user wants to encrypt in between ' or name of the file to encrypt
    3) Name of the file, to which output will be saved or - for console only output

    Function returns:
    a) 0 - if everything went ok
    b) 1 - if error occurred (wrong param count, etc.)
   ____________________________________________________________________________
*/
int main(int argc, char *argv[]) {
    /* perform check for parameters count */
    if (check_args_count(argc) != EXIT_SUCCESS) {
        printf(MAIN_WRONG_PARAM_COUNT);
        return EXIT_FAILURE;
    } else { /* parameter count ok, perform check for validity of each parameter */
        if (check_args_validity(argv) != EXIT_SUCCESS) { /* are given parameters valid? */
            return EXIT_FAILURE;
        }

        /* params are valid, continue */
        if (input == 0) { /* user wants to encrypt plaintext */
            encrypt_plaintext(input_content, output_content, key_to_use, output);
        } else if (input == 1) {
            encrypt_file(input_content, output_content, key_to_use, output);
        }
    }

    return EXIT_SUCCESS;
}
/* ____________________________________________________________________________
    int check_args_count(int argc)

    Function checks the number of parameters given to the program.
    four arguments expected (= program name, key, source, target).

    Function returns:
    a) 0 - if number of parameters == 4
    b) 1 - if number of parameters != 4
   ____________________________________________________________________________
*/
int check_args_count(int argc) {
    if (argc == 4) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

/* ____________________________________________________________________________
    int check_args_validity(char *argv[])

    Checks whether parameters given to the program by user (= program name,
    key, source, target) are valid or not.

    Function returns:
    a) 0 - if parameters are valid
    b) 1 - if parameters are not valid
   ____________________________________________________________________________
*/
int check_args_validity(char *argv[]) {
    /* first param is program name - nothing to check there... */

    /* check validity of second parameter (key) - accept text which has length > 0 */
    if (strlen(argv[1]) > 0 && strlen(argv[1]) < 33) { /* key length must be between 1 - 32 (included) */
        key_to_use = malloc(sizeof(char) * strlen(argv[1]) + 1);
        strcpy(key_to_use, argv[1]);
    } else { /* INVALID key */
        printf(MAIN_WRONG_SEC_PAR);
        return 1;
    }

    /* check validity of third parameter (input file or text) - if param contains text in between ', then text is present - else path to file is present */
    if (strlen(argv[2]) > 2 && argv[2][0] == '\'' &&
        argv[2][strlen(argv[2]) - 1] == '\'') { /* user wants to encrypt given text (in between ') */
        input = 0;
        input_content = malloc(sizeof(char) * strlen(argv[2]) + 1);
        strcpy(input_content, argv[2]);
    } else if (strlen(argv[2]) > 0) { /* user wants to encrypt given file */
        input = 1;
        input_content = malloc(sizeof(char) * strlen(argv[2]) + 1);
        strcpy(input_content, argv[2]);
    } else { /* INVALID input - zero length (path to file or text not present) */
        free(key_to_use);
        printf(MAIN_WRONG_THIRD_PAR);
        return 1;
    }

    /* check validity of fourth parameter (output file) - if param contains just "-", then output is only printed to console - else path to output file is present */
    if (strlen(argv[3]) == 1 && argv[3][0] == '-') { /* user wants to print output to console only ("-" present) */
        output = 0;
    } else if (strlen(argv[3]) > 0) { /* user wants to encrypt given file */
        output = 1;
        output_content = malloc(sizeof(char) * strlen(argv[3]) + 1);
        strcpy(output_content, argv[3]);
    } else { /* INVALID output - zero length (path to file or "-" not present) */
        free(key_to_use);
        free(input_content);
        printf(MAIN_WRONG_FOURTH_PAR);
        return 1;
    }

    return 0;
}