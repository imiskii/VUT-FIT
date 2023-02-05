/**
 * @file htab_lookup_add.c
 * @author Michal Ľaš
 * @brief Add or get item with given key from hash table
 * 
 */

#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key){
    /* Check if item with given key does not exist */
    htab_pair_t *pair = htab_find(t, key);
    if (pair != NULL){
        pair->value++;
        return pair;
    }
    /* Key is not in hash table*/
    else{
        /* Allocate new item */
        htab_item_t *new_item = malloc(sizeof(htab_item_t));
        if (new_item == NULL){
            return NULL;
        }
        htab_pair_t *new_pair = malloc(sizeof(htab_pair_t));
        if (new_pair == NULL){
            free(new_item);
            return NULL;
        }
        new_pair->key = malloc(sizeof(char) * (strlen(key) + 1));
        if (new_pair->key == NULL){
            free(new_pair);
            free(new_item);
            return NULL;
        }
        /* Fill new item */
        strcpy((void *)new_pair->key, key);
        new_pair->value = 1;
        new_item->data = new_pair;
        new_item->next = NULL;
        /* Check size limit */
        if ((t->size / t->arr_size) > AVG_LEN_MAX){
            htab_resize(t, (2 * t->arr_size));
        }
        /* Add item */
        t->size++;

        size_t index = (htab_hash_function(key) % htab_bucket_count(t));
        htab_item_t *current_item = t->arr_ptr[index];
        /* Item has synonym */
        if (current_item != NULL){
            while (current_item->next != NULL){
                current_item = current_item->next;
            }
            current_item->next = new_item;
        }
        /* Item has no synonym */
        else{
            t->arr_ptr[index] = new_item;
        }
        return new_pair;
    }
}