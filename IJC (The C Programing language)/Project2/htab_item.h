/**
 * @file htab_item.h
 * @author Michal Ľaš
 * @brief header file for hash table item
 * 
 */

#ifndef __HTAB_ITEM_H__
#define __HTAB_ITEM_H__

#include "htab.h"

#define AVG_LEN_MIN 0.5
#define AVG_LEN_MAX 2

struct htab_item{
    htab_pair_t *data;
    struct htab_item *next;
};

/* Structure htab_item */
typedef struct htab_item htab_item_t;

struct htab{
    size_t size;
    size_t arr_size;
    struct htab_item **arr_ptr;
};

#endif // __HTAB_ITEM_H__