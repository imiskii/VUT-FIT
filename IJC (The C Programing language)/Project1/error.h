/**
 * @file error.h
 * @author Michal Ľaš
 * @brief header file for error.c (Warning and error message functions)
 * 
 */

#ifndef ERROR_H
#define ERROR_H


/**
 * @brief Function print warning message
 * 
 * @param fmt 
 * @param ... 
 */
void warning_msg(const char *fmt, ...);


/**
 * @brief Function print error message and kill the program
 * 
 * @param fmt 
 * @param ... 
 */
void error_exit(const char *fmt, ...);

#endif //ERROR_H