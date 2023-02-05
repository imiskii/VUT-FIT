/**
 * @file eratosthenes.c
 * @author Michal Ľaš
 * @brief Eratosthenes algorithm for finding prime numbers
 * 
 */

#include <math.h>
#include "bitset.h"
#include "eratosthenes.h"

void Eratosthenes(bitset_t array){
    for (bitset_index_t i = 2; i < sqrt(bitset_size(array)); i++){
        if (bitset_getbit(array, i) == 0){
            for(bitset_index_t j = i*2; j < bitset_size(array); j+=i){
                bitset_setbit(array, j, 1);
            }
        }
    }
}
