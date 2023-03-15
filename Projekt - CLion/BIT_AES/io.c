#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "encrypt_logic.h"

/* ____________________________________________________________________________
    bool check_file_existence(char *filename)

    Checks whether file specified by "char *filename" exists or not.
   ____________________________________________________________________________
*/
bool check_file_existence(char *filename){
    if(access(filename, F_OK) != -1){
        return true;
    }else{
        return false;
    }
}

/* ____________________________________________________________________________
    long get_file_size(char *filename)

    Returns size of file specified by "char *filename".
   ____________________________________________________________________________
*/
long get_file_size(char *filename){
    FILE *pointer_file = NULL;
    long file_size = 0;

    pointer_file = fopen(filename, "rb");
    fseek(pointer_file, 0, SEEK_END);
    file_size = ftell(pointer_file);
    fclose(pointer_file);

    return file_size;
}

/* ____________________________________________________________________________
    char * read_file_extern(char *filename, int start_addr, int end_addr)

    Reads part of file specified by "char *filename" in binary and returns
    pointer to desired content.
   ____________________________________________________________________________
*/
char * read_file_extern(char *filename, int start_addr, int end_addr){
    FILE *pointer_file = NULL;
    char *content_file = NULL;

    pointer_file = fopen(filename, "rb");

    fseek(pointer_file, start_addr, SEEK_SET);
    content_file = malloc(end_addr - start_addr);
    fread(content_file, 1, (end_addr - start_addr), pointer_file);

    fclose(pointer_file);
    return content_file;
}

/* ___________________________________________________________________________________________
    void write_file_extern(unsigned *text_to_write, int length_to_write, char *filename)

    Writes content specified by "char *text_to_write" to file named as "char *filename".
   ___________________________________________________________________________________________
*/
void write_file_extern(unsigned char *text_to_write, int length_to_write, char *filename){
    int eof_count = length_to_write / DATA_BLOCK_SIZE;

    FILE *pointer_file = fopen(filename, "w");
    fseek(pointer_file, 0, SEEK_SET);

    int i = 0;
    int counter = 0;
    for(; i < length_to_write; i++){
        fprintf(pointer_file, "%02X",  (unsigned char) text_to_write[i] & 0xff);
        fprintf(pointer_file, " ");

        counter++;
        if(counter == DATA_BLOCK_SIZE){
            fprintf(pointer_file, "\n");
            counter = 0;
        }
    }

    fclose(pointer_file);
}