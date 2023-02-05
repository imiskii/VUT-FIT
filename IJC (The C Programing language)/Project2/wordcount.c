/**
 * @file wordcount.c
 * @author Michal Ľaš
 * @brief Count number of words occurrences in file
 */

#include <stdio.h>
#include "htab.h"
#include "htab_item.h"
#include "io.h"

#define HTAB_SIZE 99991


/**
 * @brief Function prtin key + tab + number of occurrences
 * 
 * @param pair htab_pair_t data
 */
void print_pair(htab_pair_t *pair){
    printf("%s\t%d\n", pair->key, pair->value);
}

int main(){

    htab_t *htab = htab_init(HTAB_SIZE);
    if (htab == NULL){
        fprintf(stderr, "Memmory allocation failed !\n");
        return 1;
    }


    char word[MAX_WORD_LEN];
    int word_len;

    while ((word_len = read_word(word, MAX_WORD_LEN, stdin)) != EOF){
        if (htab_lookup_add(htab, word) == NULL){
            fprintf(stderr, "Memmory allocation failed  !\n");
            htab_free(htab);
            return 1;
        }
        memset(word, '\0', MAX_WORD_LEN);
    }
    htab_for_each(htab, print_pair);
    htab_free(htab);
    return 0;
}