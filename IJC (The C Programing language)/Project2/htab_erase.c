/**
 * @file htab_erase.c
 * @author Michal Ľaš
 * @brief Delete item with given key
 * 
 */

#include <string.h>
#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


bool htab_erase(htab_t * t, htab_key_t key){
    size_t index = (htab_hash_function(key) % htab_bucket_count(t));
    htab_item_t *item = t->arr_ptr[index];
    /* Non empty array*/
    if (item != NULL){
        /* First item is searching item */
        if ((strcmp(key, item->data->key)) == 0){
            t->arr_ptr[index] = item->next;
            free((void *)item->data->key);
            free(item->data);
            free(item);
            t->size--;
            if ((t->size / t->arr_size) < AVG_LEN_MIN){
                htab_resize(t, (t->arr_size / 2));
            }
            return true;
        }
        htab_item_t *item_before = item;
        item = item->next;
        /* Next items */
        while (item != NULL){
            if ((strcmp(key, item->data->key)) == 0){
                item_before->next = item->next;
                free((void *)item->data->key);
                free(item->data);
                free(item);
                t->size--;
                if ((t->size / t->arr_size) < AVG_LEN_MIN){
                    htab_resize(t, (t->arr_size / 2));
                }
                return true;
            }
            item_before = item;
            item = item->next;
        }
        return false;
    }
    else {
        return false;
    }
}