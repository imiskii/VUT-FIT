/**
 * @file htab_find.c
 * @author Michal Ľaš
 * @brief find item in hash table
 * 
 */

#include <string.h>
#include "htab.h"
#include "htab_item.h"


htab_pair_t * htab_find(htab_t * t, htab_key_t key){
    size_t index = (htab_hash_function(key) % htab_bucket_count(t));
    htab_item_t *item = t->arr_ptr[index];

    while (item != NULL){
        if ((strcmp(key, item->data->key)) == 0){
            return item->data;
        }
        item = item->next;
    }
    return NULL;
}