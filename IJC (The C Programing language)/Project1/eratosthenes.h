/**
 * @file eratosthenes.h
 * @author Michal Ľaš
 * @brief header file for eratosthenes.c (Eratosthenes algorithm for finding prime numbers)
 * 
 */

#include "bitset.h"

#ifndef ERATOSTHENES_H
#define ERATOSTHENES_H

/**
 * @brief Eratosthenes algorithm for finding prime numbers
 * 
 * @param array bitset_t array
 */
void Eratosthenes(bitset_t array);

#endif