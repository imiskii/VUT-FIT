/**
 * @file htab_free.c
 * @author Michal Ľaš
 * @brief Delete whole hash table
 * 
 */


#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


void htab_free(htab_t * t){
    htab_clear(t);
    free(t->arr_ptr);
    free(t);
}