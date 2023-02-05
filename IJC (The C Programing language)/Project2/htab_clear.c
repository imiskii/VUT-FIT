/**
 * @file htab_clear.c
 * @author Michal Ľaš
 * @brief Delete all items in hash table
 * 
 */

#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


void htab_clear(htab_t * t){
    for (size_t i = 0; i < htab_bucket_count(t); i++){
        htab_item_t *item = t->arr_ptr[i];
        while (item != NULL){
            htab_item_t *item_to_del = item;
            item = item->next;

            free((void *)item_to_del->data->key);
            free(item_to_del->data);
            free(item_to_del);
        }
    }
    t->size = 0;
}