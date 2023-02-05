/**
 * @file bitset.h
 * @author Michal Ľaš
 * @brief header file for bitset.c, macros and inline functions
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"

#ifndef BITSET_H
#define BITSET_H

/* ============================= Generals ============================= */

/* Bit field type (for passing a parameter to a function by reference) */
typedef unsigned long bitset_t[];

/* Type index into a bit array */
typedef unsigned long bitset_index_t;

/* Unsigned long == 1 */
#define UL_1 (unsigned long) 1

/* Number of bits in one byte */
#define BYTE_SIZE 8

/* Number of bits in one unsigned long */
#define UL_SIZE_BIT (sizeof(unsigned long) * BYTE_SIZE)



/* defines and resets the array_name variable */
#define bitset_create(array_name, size)                                                                                           \
        _Static_assert(!(size <= 0), "Invalid array size !");                                                                 \
        unsigned long array_name[(size / UL_SIZE_BIT) + ((size % UL_SIZE_BIT) ? 1:0) + 1] = {size}

/* defines the array_name variable, the field will be allocated dynamically */
#define bitset_alloc(array_name,size)                                                                                             \
        assert(!(size <= 0));                                                                                                     \
        unsigned long *array_name = calloc(((size / UL_SIZE_BIT) + ((size % UL_SIZE_BIT) ? 1:0) + 1), sizeof(unsigned long)); \
        if (array_name == NULL){                                                                                                      \
            fprintf(stderr, "Memory allocation failed !");                                                                             \
            return 1;                                                                                                                 \
        }                                                                                                                             \
        *array_name = size

/* frees dynamically allocated memory in a variable array_name */
#define bitset_free(array_name) (free(array_name))

/* ================== INLINE and MACROS definitions ================== */

    #ifdef USE_INLINE
    /* INLINE FUNCTIONS */

    /**
     * @brief Function returns the declared size of the array in bits (stored in the array)
     * 
     * @param array_name bitset_t array
     * @return unsigned long size of array in bits
     */
    inline unsigned long bitset_size(bitset_t array_name){
        return array_name[0];
    }

    /**
     * @brief Function sets the specified bit in the field to the value specified by the expression
     * 
     * @param array_name bitset_t array
     * @param index index where bit will be set
     * @param expression given expression
     */
    inline void bitset_setbit(bitset_t array_name, const bitset_index_t index, const int expression){
        if ((index >= bitset_size(array_name))){
            error_exit("bit_array_getbit: Index %lu mimo rozsah 0..%lu", (bitset_index_t)index, (unsigned long)(array_name[0]-1));
        }

        expression != 0 ?  (array_name[((index) / UL_SIZE_BIT) + 1] |=  (UL_1 << ((index) % UL_SIZE_BIT))) :
                      (array_name[((index) / UL_SIZE_BIT) + 1] &= ~(UL_1 << ((index) % UL_SIZE_BIT)));
        return;
    }

    /**
     * @brief Function gets the value of the specified bit, returns the value 0 or 1
     * 
     * @param array_name bitset_t array
     * @param index index of bit
     * @return unsigned long value of bit 0 or 1
     */
    inline unsigned long bitset_getbit(bitset_t array_name, const bitset_index_t index){
        if ((index >= bitset_size(array_name))){
            error_exit("bit_array_getbit: Index %lu mimo rozsah 0..%lu", (bitset_index_t)index, (unsigned long)(array_name[0]-1));
        }

        return ((array_name[((index) / UL_SIZE_BIT) + 1] & (UL_1 << ((index) % UL_SIZE_BIT))) != 0);
    }

    #else
    /* MACROS */

    /**
     * @brief Function returns the declared size of the array in bits (stored in the array)
     * 
     * @param array_name bitset_t array
     * @return unsigned long size of array in bits
     */
    #define bitset_size(array_name) array_name[0]


    /**
     * @brief Function sets the specified bit in the field to the value specified by the expression
     * 
     * @param array_name bitset_t array
     * @param index index where bit will be set
     * @param expression given expression
     */
    #define bitset_setbit(array_name, index, expression)                                                                                        \
        if((bitset_index_t)index >= bitset_size(array_name)){                                                                              \
            error_exit("bit_array_setbit: Index %lu mimo rozsah 0..%lu", (bitset_index_t)index, (unsigned long)(array_name[0]-1));         \
        }                                                                                                                                  \
        expression != 0 ?  (array_name[(((bitset_index_t)index) / UL_SIZE_BIT) + 1] |=  (UL_1 << (((bitset_index_t)index) % UL_SIZE_BIT))) :    \
                      (array_name[(((bitset_index_t)index) / UL_SIZE_BIT) + 1] &= ~(UL_1 << (((bitset_index_t)index) % UL_SIZE_BIT)))

    
    /**
     * @brief Function gets the value of the specified bit, returns the value 0 or 1
     * 
     * @param array_name bitset_t array
     * @param index index of bit
     * @return unsigned long value of bit 0 or 1
     */
    #define bitset_getbit(array_name, index)                                                                                               \
        ((bitset_index_t)index >= bitset_size(array_name)) ?                                                                               \
        (error_exit("bit_array_getbit: Index %lu mimo rozsah 0..%lu", (bitset_index_t)index, (unsigned long)(array_name[0]-1)), -1) :      \
        ((array_name[(((bitset_index_t)index) / UL_SIZE_BIT) + 1] & (UL_1 << (((bitset_index_t)index) % UL_SIZE_BIT))) != 0)

    #endif // USE_INLINE end

#endif //BITSET_H end