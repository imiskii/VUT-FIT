/**
 * @file htab_bucket_count.c
 * @author Michal Ľaš
 * @brief Size of hash table
 * 
 */

#include "htab.h"
#include "htab_item.h"


size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}