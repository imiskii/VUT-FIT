/**
 * @file hashtable.h
 * @author Michal Ľaš
 * @brief header file for hashtable.c (Hash table)
 * 
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define MAX_HT_SIZE 101 // size of hash table


extern int HT_SIZE;

/* Element of hash table */
typedef struct ht_item {
  char *key;            // key of element
  float value;          // data
  struct ht_item *next; // pointer to synonym
} ht_item_t;

typedef ht_item_t *ht_table_t[MAX_HT_SIZE]; // hash table


/**
 * @brief Hashing function
 * 
 * @param key string with hash will be calculated
 * @return int position in hash table
 */
int get_hash(char *key);


/**
 * @brief Function initialize new hash table
 * 
 * @param table pointer to uninitialized hash table
 */
void ht_init(ht_table_t *table);


/**
 * @brief Function search for item in hash table
 * 
 * @param table pointer to initialized hash table
 * @param key key of item
 * @return ht_item_t* pointer to finded item or NULL if item with given key was not found
 */
ht_item_t *ht_search(ht_table_t *table, char *key);


/**
 * @brief Function add new item to hash table
 * 
 * @param table pointer to initialized hash table
 * @param key key of new item
 * @param data data in new item
 */
void ht_insert(ht_table_t *table, char *key, float data);


/**
 * @brief Function return data of item with key
 * 
 * @param table pointer to initialized hash table
 * @param key key of item
 * @return float* pointer to data or NULL if key was not found
 */
float *ht_get(ht_table_t *table, char *key);


/**
 * @brief Function delete item with given key and free all alocated memory
 * If there is no item with given key do nothing
 * 
 * @param table pointer to initialized hash table
 * @param key key of item that will be deleted
 */
void ht_delete(ht_table_t *table, char *key);


/**
 * @brief Function delete all items in hash table and free all alocated memory
 * 
 * @param table pointer to initialized hash table
 */
void ht_delete_all(ht_table_t *table);


#endif // HASHTABLE_H

/* END OF FILE */