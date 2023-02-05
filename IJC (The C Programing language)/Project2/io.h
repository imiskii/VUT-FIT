/**
 * @file io.h
 * @author Michal Ľaš
 * @brief header file for io.c (Read word)
 * 
 */

#include <stdio.h>

#define MAX_WORD_LEN 127

#ifndef IO_H
#define IO_H

/**
 * @brief Function read word from file 'f' with size max to buffer 's'
 * 
 * @param s buffer where word will be loaded
 * @param max max size of word
 * @param f file from which word will be read
 * @return int length of word or -1 if error or EOF
 */
int read_word(char *s, int max, FILE *f);

#endif //IO_H