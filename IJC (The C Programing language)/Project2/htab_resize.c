/**
 * @file htab_resize.c
 * @author Michal Ľaš
 * @brief Resize of existing hash table
 * 
 */

#include <stdlib.h>
#include "htab.h"
#include "htab_item.h"


void htab_resize(htab_t *t, size_t newn){
    /* New array allocation */
    htab_item_t **list = malloc(newn * sizeof(htab_item_t *));
    if (list == NULL || newn <= 0){
        /* allocation failed old hash table without changes */
        return;
    }
    else {
        /* initialization of new array to NULL */
        for (size_t i = 0; i < newn; i++){
            list[i] = NULL;
        }

        /* Iterating old hash table */
        for (size_t i = 0; i < htab_bucket_count(t); i++){
            htab_item_t *current_item = t->arr_ptr[i];
            while (current_item != NULL){
                /* count new position */
                size_t index = (htab_hash_function(current_item->data->key) % newn);

                htab_item_t *item_to_sort = current_item;
                current_item = current_item->next;
                item_to_sort->next = NULL;

                /* fill new hash table */
                htab_item_t *list_item = list[index];
                if (list_item != NULL){
                    while (list_item->next != NULL){
                        list_item = list_item->next;
                    }
                    list_item->next = item_to_sort;
                }
                else {
                    list[index] = item_to_sort;
                }
            }
        }

        /* Free old hash table and set new */
        free(t->arr_ptr);
        t->arr_ptr = list;
        t->arr_size = newn;
    }
}