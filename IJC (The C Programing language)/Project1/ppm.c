/**
 * @file ppm.c
 * @author Michal Ľaš
 * @brief Function for loading .ppm file
 * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ppm.h"
#include "error.h"


struct ppm *ppm_read(const char *filename){

    /* Open file */
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL){
        warning_msg("File %s can not be open !\n", filename);
        return NULL;
    }

    char type[3];
    unsigned xsize_given;
    unsigned ysize_given;
    unsigned char color_given;
    
    /* Loading ppm file parameters */
    if ((fscanf(fp, "%2s %u %u %hhu", type, &xsize_given, &ysize_given, &color_given)) < 4){
        warning_msg("Wrong format or damaged file %s !\n", filename);
        fclose(fp);
        return NULL;
    }

    /* file check */
    size_t current_size = xsize_given * ysize_given * 3;

    if (current_size > MAX_SIZE){
        warning_msg("Too big image resolution %s !\n",filename);
        fclose(fp);
        return NULL;
    }

    if (strcmp(type, "P6")){
        warning_msg("Wrong file variant %s ! (expected : 'P6')\n", filename);
        fclose(fp);
        return NULL;
    }

    /* Memory allocation */
    struct ppm *ppm_data = malloc(sizeof(struct ppm) + current_size);
    if (ppm_data == NULL){
        warning_msg("Memory allocation failed !\n");
        fclose(fp);
        return NULL;
    }

    /* Loading image data */
    size_t read_check = fread(ppm_data->data, sizeof(char), current_size, fp);
    if (read_check != current_size){
        warning_msg("Error while reading the file %s !\n", filename);
        fclose(fp);
        ppm_free(ppm_data);
        return NULL;
    }
    fclose(fp);

    ppm_data->xsize = xsize_given;
    ppm_data->ysize = ysize_given;

    return ppm_data;
}


void ppm_free(struct ppm *p){
    free(p);
}