/**
 * @file primes.c
 * @author Michal Ľaš
 * @brief printing last PRINT_COUNT number of primes from range 0 to SIZE
 * 
 */

#include <time.h>
#include <stdio.h>
#include "bitset.h"
#include "eratosthenes.h"

#define SIZE 300000000
#define PRINT_COUNT 10


int main(void){

    /* TIME */
    double time1;
    time1 = (double) clock();

    /* Eratosthenes */
    bitset_create(pole, SIZE);

    Eratosthenes(pole);


    /* Searching last PRINT_COUNT primes from range SIZE */
    unsigned long result[PRINT_COUNT];
    unsigned count = 0;

    for (bitset_index_t i = SIZE - 1; i > 1; i--){
        if ((bitset_getbit(pole, i) == 0)){
            result[count] = i;
            count++;
            if (count == PRINT_COUNT){
            break;
            }
        }
    }

    for (int i = PRINT_COUNT; i > 0; i--){
        printf("%lu\n", result[i - 1]);
    }
 
    time1 = time1 / CLOCKS_PER_SEC;
    fprintf(stderr, "Time=%.3g\n", (((double) clock()) / CLOCKS_PER_SEC) - time1);

    return 0;
}