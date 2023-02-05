/**
 * @file htab_hash_function.c
 * @author from site: http://www.fit.vutbr.cz/study/courses/IJC/public/DU2.html 
 * @brief hashing function
 * 
 */


#include <stdint.h>
#include "htab.h"
#include "htab_item.h"


size_t htab_hash_function(htab_key_t str) {
    uint32_t h = 0;
    const unsigned char *p;
    for(p = (const unsigned char*)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}