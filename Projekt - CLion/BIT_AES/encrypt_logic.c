#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encrypt_logic.h"
#include "io.h"
#include "resources.h"

/* ___________________________________________________________________________________________
    void key_expansion_core(unsigned char *sequence, unsigned char it_index)

    Performs left rotation and sbox substitution of given sequence (4B) + xor and rcon substitution
    is performed on first byte of the sequence.

    Params:
    sequence = 4B sequence, which will be modified
    it_index = iteration index
   ___________________________________________________________________________________________
*/
void key_expansion_core(unsigned char *sequence, unsigned char it_index){
    unsigned char modified[4]; /* modified sequence */

    /* perform left rotation - START */
    modified[0] = sequence[1];
    modified[1] = sequence[2];
    modified[2] = sequence[3];
    modified[3] = sequence[0];
    /* perform left rotation - END */

    /* sbox substitution - START */
    int i = 0;
    for(; i < 4;i++){
        modified[i] = s_box[modified[i]];
    }
    /* sbox substitution - END */

    modified[0] = modified[0] ^ r_con[it_index]; /* xor and rcon substitution on first sequence byte */

    /* put sequence back - START */
    i = 0;
    for(; i < 4; i++){
        sequence[i] = modified[i];
    }
    /* put sequence back - END */
}

/* ___________________________________________________________________________________________
    unsigned char * key_expansion(unsigned char *original_key)

    Expands key given by user to 176B. That means, that 10 new keys are created during this
    process if size of the original key is 16B.

    Params:
    original_key = unexpanded key, which we are going to expand
   ___________________________________________________________________________________________
*/
unsigned char * key_expansion(unsigned char *original_key){
    unsigned char *expanded_key = malloc(sizeof(unsigned char) * EXPANDED_KEY_SIZE);

    /* copy first 16 bytes from original key */
    strncpy(expanded_key, original_key, DATA_BLOCK_SIZE);
    int used_bytes = DATA_BLOCK_SIZE; /* number of already created bytes for expanded key */

    int rcon_it_count = 1; /* rcon iteration counter */
    while(used_bytes < EXPANDED_KEY_SIZE){ /* generate 176 bytes total for expanded key */
        unsigned char last_4B_gen[4]; /* temp storage - 4B */

        /* read last 4 generated bytes */
        int i = 0;
        for(; i < 4; i++){
            last_4B_gen[i] = expanded_key[i + used_bytes - 4];
        }

        /* core expansion each 16 bytes (new key generated) */
        if(used_bytes % DATA_BLOCK_SIZE == 0){
            key_expansion_core(last_4B_gen, rcon_it_count);
            rcon_it_count++; /* increment for next iteration */
        }

        /* xor with first 4B generated in 16B key with last 4B generated */
        i = 0;
        for(; i < 4; i++) {
            expanded_key[used_bytes] = expanded_key[used_bytes - DATA_BLOCK_SIZE] ^ last_4B_gen[(unsigned char) i];
            used_bytes++;
        }
    }

    return expanded_key;
}

/* ___________________________________________________________________________________________
    void sub_bytes(unsigned char *data_block)

    Replaces each byte in given data block (16B) with corresponding value from
    Rijndael s-box lookup table.

    Params:
    data_block = 16B block, in which bytes will be replaced by corresponding values from sbox
   ___________________________________________________________________________________________
*/
void sub_bytes(unsigned char *data_block){
    /* replace each byte in the data block with the corresponding value from the sbox - START */
    int i = 0;
    for(; i < DATA_BLOCK_SIZE; i++){
        data_block[i] = s_box[data_block[i]];
    }
    /* replace each byte in the data block with the corresponding value from the sbox - END */
}

/* ___________________________________________________________________________________________
    void shift_rows(unsigned char *data_block)

    Performs row shifting, when encrypting with AES rows must be shifted to left by:
    0 - in case of first row = first row is not shifted at all
    1 - in case of second row = second row is shifted by one to left
    2 - in case of third row = third row is shifted by two to left
    3 - in case of fourth row = fourth row is shifted by three to left

    Params:
    data_block = 16B block (sometimes called state), in which shifting modifications will be performed
   ___________________________________________________________________________________________
*/
void shift_rows(unsigned char *data_block){
    /* create array which will contain shifted rows */
    unsigned char modifiedDataBlock[DATA_BLOCK_SIZE];

    /* shift first row - START */
    modifiedDataBlock[0] = data_block[0];
    modifiedDataBlock[4] = data_block[4];
    modifiedDataBlock[8] = data_block[8];
    modifiedDataBlock[12] = data_block[12];
    /* shift first row - END */

    /* shift second row - START */
    modifiedDataBlock[1] = data_block[5];
    modifiedDataBlock[5] = data_block[9];
    modifiedDataBlock[9] = data_block[13];
    modifiedDataBlock[13] = data_block[1];
    /* shift second row - END */

    /* shift third row - START */
    modifiedDataBlock[2] = data_block[10];
    modifiedDataBlock[6] = data_block[14];
    modifiedDataBlock[10] = data_block[2];
    modifiedDataBlock[14] = data_block[6];
    /* shift third row - END */

    /* shift fourth row - START */
    modifiedDataBlock[3] = data_block[15];
    modifiedDataBlock[7] = data_block[3];
    modifiedDataBlock[11] = data_block[7];
    modifiedDataBlock[15] = data_block[11];
    /* shift fourth row - END */

    int i = 0;
    for(; i < DATA_BLOCK_SIZE; i++){
        data_block[i] = modifiedDataBlock[i];
    }
}

/* ___________________________________________________________________________________________
    void mix_columns(unsigned char *data_block)

    Performs mix columns step. Basically takes care of multiplication of bytes in state (16B data block)
    array with other bytes.

    Params:
    data_block = 16B block, which will be modified
   ___________________________________________________________________________________________
*/
void mix_columns(unsigned char *data_block){
    unsigned char modifiedDataBlock[DATA_BLOCK_SIZE];

    int i = 0;
    /* calculate first row - START */
    for(; i < DATA_BLOCK_SIZE; i += 4){
        modifiedDataBlock[i] = (unsigned char)(gal_mul2[data_block[i]] ^ gal_mul3[data_block[i + 1]] ^ data_block[i + 2] ^ data_block[i + 3]);
    }
    /* calculate first row - END */

    /* calculate second row - START */
    i = 1;
    for(; i < DATA_BLOCK_SIZE; i += 4){
        modifiedDataBlock[i] = (unsigned char)(data_block[i - 1] ^ gal_mul2[data_block[i]] ^ gal_mul3[data_block[i + 1]] ^ data_block[i + 2]);
    }
    /* calculate second row - END */

    /* calculate third row - START */
    i = 2;
    for(; i < DATA_BLOCK_SIZE; i += 4){
        modifiedDataBlock[i] = (unsigned char)(data_block[i - 2] ^ data_block[i - 1] ^ gal_mul2[data_block[i]] ^ gal_mul3[data_block[i + 1]]);
    }
    /* calculate third row - END */

    /* calculate fourth row - START */
    i = 3;
    for(; i < DATA_BLOCK_SIZE; i += 4){
        modifiedDataBlock[i] = (unsigned char)(gal_mul3[data_block[i - 3]] ^ data_block[i - 2] ^ data_block[i - 1] ^ gal_mul2[data_block[i]]);
    }
    /* calculate fourth row - END */

    i = 0;
    for(; i < DATA_BLOCK_SIZE; i++){
        data_block[i] = modifiedDataBlock[i];
    }
}

/* ___________________________________________________________________________________________
    void add_round_key(unsigned char *state, unsigned char *round_key)

    Performs XOR operations with data block byte and corresponding round key byte.

    Params:
    state - basically 16B data block
    round_key - round key with which state will be XORed
   ___________________________________________________________________________________________
*/
void add_round_key(unsigned char *state, unsigned char *round_key){
    int i = 0;
    for(; i < DATA_BLOCK_SIZE; i++){
        state[i] = state[i] ^ round_key[i];
    }
}

/* ____________________________________________________________________________
    void encrypt_file(unsigned char *path_to_file_input,
    unsigned char *path_to_file_output, unsigned char *key, int output)

    Encrypts file in given path with given key. If output == 0,
    then output is printed to console only, else file in location given by
    path_to_file_output is created.

    Params:
    path_to_file_input - path to file, which user wants to encrypt
    path_to_file_output - path to file, to which output will be saved
    key - user given key with which file will be encrypted
    output - specifies mode (to console / file)
   ____________________________________________________________________________
*/
void encrypt_file(unsigned char *path_to_file_input, unsigned char *path_to_file_output, unsigned char *key, int output){
    bool file_exists = check_file_existence(path_to_file_input);
    if(file_exists == false){
        printf(FILE_NOT_EXISTS, path_to_file_input);
        return;
    }

    long file_size = get_file_size(path_to_file_input); /* retrieve size of file */
    char *file_content = read_file_extern(path_to_file_input, 0, file_size);

    if(file_size % DATA_BLOCK_SIZE != 0){ /* add padding to last data block */
        int size_to_read = (file_size / DATA_BLOCK_SIZE + 1) * DATA_BLOCK_SIZE; /* number of blocks in file (16B each) */
        int zeros_to_add_pad = size_to_read - file_size; /* number of 0 (padding), which needs to be added to last block */

        file_content = realloc(file_content, file_size + zeros_to_add_pad);

        int i = 0;
        for(; i < zeros_to_add_pad; i++){
            file_content[file_size + i] = 0;
        }

        file_size += zeros_to_add_pad;
    }

    unsigned char *expanded_key = key_expansion(key);

    int i = 0;
    for(; i < file_size / DATA_BLOCK_SIZE; i++){
        /* perform encryption of one data block (16b) */
        encrypt_state(file_content + (i * DATA_BLOCK_SIZE), expanded_key); /* go after 16B blocks */
    }

    if(output == 0){ /* output to console only */
        i = 0;
        int counter = 0;
        for(; i < file_size; i++){
            printf("%02X", (unsigned char) file_content[i] & 0xff);
            printf(" ");
            counter++;
            if(counter == DATA_BLOCK_SIZE){
                printf("\n");
                counter = 0;
            }
        }
    }else if(output == 1){ /* print result to file */
        write_file_extern(file_content, file_size, path_to_file_output);
    }

    free(file_content);
    free(expanded_key);
}

/* ____________________________________________________________________________
    void encrypt_plaintext(unsigned char *plaintext,
    unsigned char *path_to_file_output, unsigned char *key, int output)

    Encrypts plaintext with given key. If output == 0,
    then output is printed to console only, else file in location given by
    path_to_file_output is created.

    Params:
    plaintext - char sequence, which user wants to encrypt
    path_to_file_output - path to file, to which output will be saved
    key - user given key with which file will be encrypted
    output - specifies mode (to console / file)
   ____________________________________________________________________________
*/
void encrypt_plaintext(unsigned char *plaintext, unsigned char *path_to_file_output, unsigned char *key, int output){
    /* remove first and last char from the plaintext (text is between '') - START */
    char *plaintext_to_encrypt = malloc(sizeof(char) * strlen(plaintext) - 2 + 1); /* -2 because of ''; +1 for end character */
    int i = 0;
    int plaintext_to_encrypt_length = 0;

    for(; i < strlen(plaintext); i++){
        if((i != 0) && i != (strlen(plaintext) - 1)){ /* if not first or last character (''), then copy */
            plaintext_to_encrypt[i - 1] = plaintext[i];
            plaintext_to_encrypt_length++;
        }
    }
    /* remove first and last char from the plaintext (text is between '') - END */

    /* plaintext must be divided to blocks of 16B each, add padding if not possible - START */

    if(plaintext_to_encrypt_length % DATA_BLOCK_SIZE != 0){ /* add padding */
        int original_length = plaintext_to_encrypt_length;

        plaintext_to_encrypt_length = (plaintext_to_encrypt_length / DATA_BLOCK_SIZE + 1) * DATA_BLOCK_SIZE;
        plaintext_to_encrypt = realloc(plaintext_to_encrypt, sizeof(char) * plaintext_to_encrypt_length);

        i = original_length;
        for(; i < plaintext_to_encrypt_length; i++){
            plaintext_to_encrypt[i] = 0;
        }
    }
    /* plaintext must be divided to blocks of 16B each, add padding if not possible - END */

    unsigned char *expanded_key = key_expansion(key);

    i = 0;
    for(; i < plaintext_to_encrypt_length / DATA_BLOCK_SIZE; i++){
        /* perform encryption of one data block (16B) */
        encrypt_state(plaintext_to_encrypt + (i * DATA_BLOCK_SIZE), expanded_key); /* go after 16B blocks */
    }

    if(output == 0){ /* output to console only */
        i = 0;
        int counter = 0;
        for(; i < plaintext_to_encrypt_length; i++){
            printf("%02X", (unsigned char) plaintext_to_encrypt[i] & 0xff);
            printf(" ");

            counter++;
            if(counter == DATA_BLOCK_SIZE){
                printf("\n");
                counter = 0;
            }
        }
    }else if(output == 1){ /* print result to file */
        write_file_extern(plaintext_to_encrypt, plaintext_to_encrypt_length, path_to_file_output);
    }

    free(expanded_key);
    free(plaintext_to_encrypt);
}


/* ____________________________________________________________________________
    void encrypt_state(unsigned char *block_to_encrypt, unsigned char *expanded_key)

    Performs encryption of one data block (16B)

    Params:
    block_to_encrypt - 16B block which is going to be encrypted by AES
    expanded_key - key expanded to 176B
   ____________________________________________________________________________
*/
void encrypt_state(unsigned char *block_to_encrypt, unsigned char *expanded_key){
    add_round_key(block_to_encrypt, expanded_key);

    /* perform sub_bytes, shift_rows, mix_columns and add_round_key for each round (except last round) */
    int round_index = 0;
    for(; round_index < (TOTAL_ROUNDS_NUM - 1); round_index++){
        sub_bytes(block_to_encrypt);
        shift_rows(block_to_encrypt);
        mix_columns(block_to_encrypt);
        add_round_key(block_to_encrypt, expanded_key + (DATA_BLOCK_SIZE * (round_index + 1))); /* go after 16B blocks */
    }

    /* for final round perform just sub_bytes(), shift_rows() and add_round_key() */
    sub_bytes(block_to_encrypt);
    shift_rows(block_to_encrypt);
    add_round_key(block_to_encrypt, expanded_key + 160);
}