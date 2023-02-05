/**
 * @file htab_for_each.c
 * @author Michal Ľaš
 * @brief Call function for all items in hash table
 * 
 */

#include "htab.h"
#include "htab_item.h"


void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)){
    for (size_t i = 0; i < htab_bucket_count(t); i++){
        htab_item_t *item = t->arr_ptr[i];
        while (item != NULL){
            f(item->data);
            item = item->next;
        }
    }
}