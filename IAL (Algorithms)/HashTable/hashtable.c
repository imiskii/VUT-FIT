/**
 * @file hashtable.c
 * @author Michal Ľaš
 * @brief Hash Table - implemented using static array
 * 
 */

#include "hashtable.h"


int HT_SIZE = MAX_HT_SIZE;


int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}


void ht_init(ht_table_t *table) {
  for (size_t index = 0; index < HT_SIZE; index++){
    (*table)[index] = NULL;
  }
  
}


ht_item_t *ht_search(ht_table_t *table, char *key) {

  ht_item_t *item = (*table)[get_hash(key)];

  while (item != NULL){
    if (item->key == key){
      return item;
    }
    item = item->next;
  }
  return NULL;
}


void ht_insert(ht_table_t *table, char *key, float value) {

  ht_item_t *item = ht_search(table, key);

  if (item == NULL){

    ht_item_t *new = malloc(sizeof(struct ht_item));
    if (new == NULL){
      return;
    }
    int index = get_hash(key);
    new->key = key;
    new->value = value;
    new->next = (*table)[index];
    (*table)[index] = new;
  }
  else{
    item->value = value;
  }
}


float *ht_get(ht_table_t *table, char *key) {

  ht_item_t *item = ht_search(table, key);

  if (item != NULL){
    return &item->value;
  }
  return NULL;
}


void ht_delete(ht_table_t *table, char *key) {

  int index = get_hash(key);

  ht_item_t *item = (*table)[index];
  ht_item_t *tmp = item;               // item to be deleted

  while (tmp != NULL){                 // find item
    if (tmp->key == key){

      if (tmp->next != NULL){          // it has synonym
        item->next = tmp->next;
      }
      else{
        item->next = NULL;             // it has no synonym
      }

      if (item == tmp){                // deleted item is first item
        (*table)[index] = tmp->next;
      }

      //free(tmp->key);
      free(tmp);
      return;
    }
    item = tmp;
    tmp = item->next;
  }

  return;
}


void ht_delete_all(ht_table_t *table) {

  ht_item_t *item;
  ht_item_t *tmp;

  for (size_t index = 0; index < HT_SIZE; index++){

    item = (*table)[index];

    while (item != NULL){
      tmp = item;
      item = item->next;

      //free(item->key);
      free(tmp);
    }
    (*table)[index] = NULL;
  }
}
