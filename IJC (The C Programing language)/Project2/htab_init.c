/**
 * @file htab_init.c
 * @author Michal Ľaš
 * @brief Initialization of new hash table
 * 
 */

#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


htab_t *htab_init(size_t n){
    htab_t *new = malloc(sizeof(htab_t));
    if (new == NULL){
        return NULL;
    }
    htab_item_t **list = malloc(n * sizeof(htab_item_t *));
    if (list == NULL){
        return NULL;
    }
    new->size = 0;
    new->arr_size = n;
    new->arr_ptr = list;
    for (size_t i = 0; i < n; i++){
        new->arr_ptr[i] = NULL;
    }
    return new;
}