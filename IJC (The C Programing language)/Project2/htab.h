/**
 * @file htab.h
 * @author from site: http://www.fit.vutbr.cz/study/courses/IJC/public/DU2.html
 * @brief header file for hash table functions
 * 
 */


#ifndef __HTAB_H__
#define __HTAB_H__

#include <string.h>     // size_t
#include <stdbool.h>    // bool

// Table:
struct htab;    // uncomplete declaration of structure
typedef struct htab htab_t;

// Types:
typedef const char * htab_key_t;        // type key
typedef int htab_value_t;               // type value

// Data in Table:
typedef struct htab_pair {
    htab_key_t    key;          // key
    htab_value_t  value;        // value
} htab_pair_t;


/**
 * @brief Hashing function
 * 
 * @param str string/key that will be hashed
 * @return size_t index to hash table
 */
size_t htab_hash_function(htab_key_t str);


/**
 * @brief Function initialize new dynamicly alocated hashing table
 * 
 * @param n size of new hash table
 * @return htab_t* pointer to new created hash table
 */
htab_t *htab_init(size_t n);


/**
 * @brief Function count items in hash table
 * 
 * @param t pointer to htab_t - hash table
 * @return size_t count of items in hash table
 */
size_t htab_size(const htab_t * t);


/**
 * @brief Function return current size of hash table
 * 
 * @param t pointer to htab_t - hash table
 * @return size_t size of hash table
 */
size_t htab_bucket_count(const htab_t * t);


/**
 * @brief Function resize hash table and move all items to new hash table
 * 
 * @param t pointer to hash table that will be resized
 * @param newn new size of hash table
 */
void htab_resize(htab_t *t, size_t newn);


/**
 * @brief Function find item in hash table
 * 
 * @param t pointer to initialized hash table
 * @param key searching key
 * @return htab_pair_t* pointer to item with given key or NULL if not found
 */
htab_pair_t * htab_find(htab_t * t, htab_key_t key);


/**
 * @brief add new item to hash table
 * - If item with same key exist function return pointer to that item
 * 
 * @param t pointer to initialized hash table
 * @param key key of item
 * @return htab_pair_t* pointer to item with given key
 */
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);


/**
 * @brief Delete item with given key and free all allocated memmory 
 * 
 * @param t pointer to initialized hash table
 * @param key key of item to be deleted
 * @return true if item was in hash table
 * @return false if item was not in hash table
 */
bool htab_erase(htab_t * t, htab_key_t key);


/**
 * @brief Function iterate through hash table and for all items call function 'f'
 * 
 * @param t pointer to initialized hash table
 * @param f function that will be called for all items
 */
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));


/**
 * @brief Function delete all items in hash table and free allocated memmory
 * 
 * @param t pointer to initialized hash table
 */
void htab_clear(htab_t * t);


/**
 * @brief Function free all items in hash table and free also hash table
 * 
 * @param t pointer to initialized hash table
 */
void htab_free(htab_t * t);

#endif // __HTAB_H__