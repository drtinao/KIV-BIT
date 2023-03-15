#ifndef BIT_AES_ENCRYPT_LOGIC_H
#define BIT_AES_ENCRYPT_LOGIC_H

#define EXPANDED_KEY_SIZE 176
#define DATA_BLOCK_SIZE 16
#define TOTAL_ROUNDS_NUM 10
#define FILE_NOT_EXISTS "Input file with name %s does not exist, exiting...\n"

/* ____________________________________________________________________________

    Function prototypes
   ____________________________________________________________________________
*/
void encrypt_file(unsigned char *path_to_file_input, unsigned char *path_to_file_output, unsigned char *key, int output);
void encrypt_plaintext(unsigned char *plaintext, unsigned char *path_to_file_output, unsigned char *key, int output);
void encrypt_state(unsigned char *block_to_encrypt, unsigned char *key);
void key_expansion_core(unsigned char *in, unsigned char i);
unsigned char * key_expansion(unsigned char *input_key);
void sub_bytes(unsigned char *state);
void shift_rows(unsigned char *state);
void mix_columns(unsigned char *state);
void add_round_key(unsigned char *state, unsigned char *round_key);

#endif //BIT_AES_ENCRYPT_LOGIC_H