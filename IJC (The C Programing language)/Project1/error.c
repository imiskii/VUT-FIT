/**
 * @file error.c
 * @author Michal Ľaš
 * @brief Warning and error message functions
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"


void warning_msg(const char *fmt, ...){
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}


void error_exit(const char *fmt, ...){
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(1);
}