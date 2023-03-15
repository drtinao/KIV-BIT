#ifndef BIT_AES_IO_H
#define BIT_AES_IO_H

#include <stdbool.h>

/* ____________________________________________________________________________

    Function prototypes
   ____________________________________________________________________________
*/
bool check_file_existence(char *filename);
long get_file_size(char *filename);
char * read_file_extern(char *filename, int start_addr, int end_addr);
void write_file_extern(unsigned char *text_to_write, int length_to_write, char *filename);

#endif //BIT_AES_IO_H