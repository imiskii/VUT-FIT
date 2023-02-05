/**
 * @file io.c
 * @author Michal Ľaš
 * @brief Read word
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include "htab.h"
#include "io.h"



int read_word(char *s, int max, FILE *f){

    static short wm;

    /* Check parameters */
    if (f == NULL || max <= 0){
        fprintf(stderr, "ERROR while processing word !");
        return -1;
    }

    int symbol;
    /* Skip spaces */
    while((symbol = fgetc(f)) != EOF && isspace(symbol));

    if (symbol == EOF){
        return EOF;
    }

    int count = 0;
    s[count] = symbol;
    count++;

    /* Loading 127 symbols to buffer s */
    for (;count < (max - 1) ;){
        symbol = fgetc(f);
        if (isspace(symbol) || symbol == EOF){
            break;
        }

        s[count] = symbol;
        count++;
    }

    s[count] = '\0';

    /* Loading rest after already proccess 127 symbols */
    if (count == (max - 1)){
        while ((symbol = fgetc(f)) != EOF){
            if (isspace(symbol)){
                break;
            }
            count++;
            if (count >= MAX_WORD_LEN && !wm){
                fprintf(stderr, "Too long word (max 127 symbols) !\n");
                wm = 1;
            }
        }
    }
    return count;
}