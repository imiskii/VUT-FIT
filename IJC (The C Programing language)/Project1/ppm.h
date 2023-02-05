/**
 * @file ppm.h
 * @author Michal Ľaš
 * @brief header file for ppm.c (Function for loading .ppm file)
 * 
 */

#ifndef PPM_H
#define PPM_H

#define MAX_SIZE (8000 * 8000 * 3) // maximal resolution

struct ppm {
        unsigned xsize;
        unsigned ysize;
        char data[];    // RGB byts, 3*xsize*ysize
    };


/**
 * @brief Function loads data from .ppm file to dynamicly allocated struct ppm
 * 
 * @param filename name of file
 * @return struct ppm* pointer to struct ppm with loaded data
 */
struct ppm * ppm_read(const char * filename);


/* Uvolni pamet dynamycky alokovanu pomocou funkcie ppm_read */
/**
 * @brief Function free all dynamic alocated memory in struct ppm
 * 
 * @param p struct ppm
 */
void ppm_free(struct ppm *p);

#endif //PPM_H