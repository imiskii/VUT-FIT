/**
 * @file bitset.c
 * @author Michal Ľaš
 * @brief Inline function for bitset
 * 
 */


#include "bitset.h"

#ifdef USE_INLINE

unsigned long bitset_size(bitset_t array_name);
void bitset_setbit(bitset_t array_name, const unsigned long index, const int expression);
unsigned long bitset_getbit(bitset_t array_name, const unsigned long index);

#endif // USE_INLINE end