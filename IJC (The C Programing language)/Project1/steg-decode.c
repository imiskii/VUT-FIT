/**
 * @file steg-decode.c
 * @author Michal Ľaš
 * @brief finding steganography hidden secret message in ppm file, that is located on prime numbers
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "error.h"
#include "ppm.h"
#include "eratosthenes.h"
#include "bitset.h"

#define getbit(array_name, index) ((array_name[(index / UL_SIZE_BIT)] & (UL_1 << (index % UL_SIZE_BIT))) != 0)
#define setbit(array_name, index, expression) (expression != 0 ?  (array_name |=  (UL_1 << (index % UL_SIZE_BIT))) : \
                                                        (array_name &= ~(UL_1 << (index % UL_SIZE_BIT))))

int main(int argc, char *argv[]){

    /* Check program arguments */
    if (argc != 2){
        error_exit("Program arguments ERROR !\n");
    }

    /* Loading ppm file (image) */
    struct ppm *data_file;

    data_file = ppm_read(argv[1]);
    if (data_file == NULL){
        error_exit("ERROR while loading the file !\n");
    }

    /* Finding prime numbers by size (resolution) of image */
    unsigned bit_array_size = data_file->xsize * data_file->ysize * 3;

    bitset_alloc(bit_array, bit_array_size);

    Eratosthenes(bit_array);

    /* Searching and printing hidden message from ppm file */
    char result = 0;
    bitset_index_t pos = 0;

    for (bitset_index_t i = 27; i < bit_array_size; i++){
        if (bitset_getbit(bit_array, i) == 0){
            setbit(result, pos, getbit((&(data_file->data[i + 1])), 0));
            pos++;
            if (pos >= 8){
                if (result == '\0'){
                    printf("\n");
                    break;
                }
                printf("%c", result);
                pos = 0;
                result = 0;
            }
        }
    }
    /* Finded message */
    if (result == '\0'){
        bitset_free(bit_array);
        ppm_free(data_file);
        return 0;
    }
    /* Message was not found */
    bitset_free(bit_array);
    ppm_free(data_file);
    error_exit("Message was not found !\n");
}