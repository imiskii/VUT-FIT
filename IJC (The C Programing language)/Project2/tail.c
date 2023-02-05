/**
 * @file tail.c
 * @author Michal Ľaš
 * @brief Tail
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LEN 4095

/* ************************************************** */
/* ==================== Prototypes ================== */
/* ************************************************** */ 

void load_args(int argc, char *argv[], size_t *lines_to_print, size_t *file_name_index);
bool isdigit_string(char string[]);
FILE *openfile(char file_name[], size_t control_index);
size_t load_lines(char *c_buffer[], size_t n, FILE *fp, short wm);
void print_lines_and_free(char *c_buffer[], size_t n, size_t head);

/* ************************************************** */
/* ===================== MAIN ======================= */
/* ************************************************** */

int main(int argc, char *argv[]){

    static short wm; // warning message
    size_t lines_to_print = 10;
    size_t file_name_index = 0;

    /* Argument processing */

    load_args(argc, argv, &lines_to_print, &file_name_index);
    if (lines_to_print <= 0){
        return 0;
    }

    /* Reading file */

    FILE *file_ptr = openfile(argv[file_name_index], file_name_index);

    /* Cyclic buffer for loading lines */

    char *c_buffer[lines_to_print];
    for (size_t i = 0; i < lines_to_print; i++){
        c_buffer[i] = calloc(MAX_LINE_LEN, 1);
        if (c_buffer[i] == NULL){
            fprintf(stderr, "Memmory allocation error !\n");
            return 1;
        }
    }

    /* Iterating through lines using cyclic buffer  */

    size_t head = load_lines(c_buffer, lines_to_print, file_ptr, wm);

    /* print last 'lines_to_print' lines */

    print_lines_and_free(c_buffer, lines_to_print, head);

    /* close file */

    fclose(file_ptr);

    return 0;
}

/* ************************************************* */
/* =================== FUNCTIONS =================== */
/* ************************************************* */


/**
 * @brief Function process program arguments
 * 
 * @param argc count of arguments
 * @param argv arguments
 * @param lines_to_print how many lines will be printed 
 * @param file_name_index pointer where function return index of file name from arguments
 */
void load_args(int argc, char *argv[], size_t *lines_to_print, size_t *file_name_index){

    if (argc > 4){
        fprintf(stderr, "Too many arguments !\n");
    }

    for (int i = 1; i < argc; i++){
        if ((strcmp(argv[i], "-n")) == 0){
            if (i+1 >= argc){
                fprintf(stderr, "Invalid argument -n !\n");
                exit(1);
            }
            if (!isdigit_string(argv[i+1])){
                fprintf(stderr, "Invalid argument -n !\n");
                exit(1);
            }
            *lines_to_print = atoi(argv[i+1]);
            i++;
        }
        else if (*file_name_index == 0){
            *file_name_index = i;
        }
        else{
            fprintf(stderr, "Invalid arguments !\n");
        }
    }
}


/**
 * @brief Function check if there are numbers in string
 * 
 * @param string string
 * @return true if there are only numbers
 * @return false if there are other symbol than number
 */
bool isdigit_string(char string[]){
    for (size_t i = 0; i < strlen(string); i++){
        if(!isdigit(string[i])){
            return false;
        }
    }
    return true;
}


/**
 * @brief Function for opening file
 * 
 * @param file_name name of file that will be opened
 * @param control_index if as control index is set 0, then imput is taken from stdin
 * @return FILE* pointer to opened file
 */
FILE *openfile(char file_name[], size_t control_index){
    FILE *file_ptr;
    if (control_index == 0){
        return stdin;
    }
    else{
        file_ptr = fopen(file_name, "r");
        if (file_ptr == NULL){
            fprintf(stderr, "Error while opening file!\n");
            exit(1);
        }
        return file_ptr;
    }
}


/**
 * @brief Function load 'n' last lines from file/stdin to cyclic buffer
 * 
 * @param c_buffer pointer to cyclic buffer
 * @param n how many lines
 * @param fp pointer to file/stdin
 * @param wm warning message
 * @return size_t position (index) of 'head' to c_buffer
 */
size_t load_lines(char *c_buffer[], size_t n, FILE *fp, short wm){
    size_t head = 0;
    while ((fgets(c_buffer[head], MAX_LINE_LEN, fp)) != NULL){
        size_t line_len = strlen(c_buffer[head]);;
        if ((line_len == MAX_LINE_LEN - 1) && (c_buffer[head][line_len - 1] != '\n')){
            /* Warning msg -> vypíše sa len raz */
            if (!wm) {
                fprintf(stderr, "Too long line was read !\n");
                wm = 1;
            }

            c_buffer[head][line_len - 1] = '\n';

            /* Preskočenie zvyšku riadku */
            char trash[MAX_LINE_LEN];
            do {
                if (fgets(trash, MAX_LINE_LEN, fp) == NULL) break;
            } while ((strlen(trash) == MAX_LINE_LEN - 1) && (trash[MAX_LINE_LEN - 2] != '\n'));
        }
        head = (head + 1) % n;
    }
    return head;
}


/**
 * @brief Function print last 'n' lines from c_buffer
 * Function also free alocated memmory
 * 
 * @param c_buffer Cyclic buffer
 * @param n how many lines will be printed
 * @param head from which index in buffer function will be printing
 */
void print_lines_and_free(char *c_buffer[], size_t n, size_t head){
    for (size_t i = 0; i < n; i++){
        printf("%s", c_buffer[head]);
        free(c_buffer[head]);
        head = (head + 1) % n;
    }
}