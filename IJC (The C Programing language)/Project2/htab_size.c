/**
 * @file htab_size.c
 * @author Michal Ľaš
 * @brief Count items in hash table
 * 
 */

#include "htab.h"
#include "htab_item.h"


size_t htab_size(const htab_t * t){
    return t->size;
}