/**
 * @file errors.hpp
 * @author Michal Ľaš
 * @brief header file for errors.cpp
 * @date 2023-19-10
 * 
 */

#include <iostream>


#ifndef ERRORS_H
#define ERRORS_H


using namespace std;


// Enumartion of possible error codes
enum class ErrorCodes
{
    SUCCESS = 0,
    FAILURE = 1,
    PROG_ARG_ERR = 2,
    OPEN_PCAP_ERR = 10,
    PCAP_COMPILE_ERR = 11,
    PCAP_SETFILTER_ERR = 12,
    PCAP_LOOP_ERR = 13,
};



/**
 * @brief Function print the error message to STDERR and exit program with given exit code
 * 
 * @tparam Args 
 * @param error_code program exit error code
 * @param args Error message
 */
template <class... Args>
inline void exit_error(ErrorCodes error_code, Args... args)
{
    (cerr << "ERR: " <<... << args) << endl;
    exit(static_cast<int>(error_code));
}


/**
 * @brief Function print the warning message to STDERR
 * 
 * @tparam Args 
 * @param args Warning message
 */
template <class... Args>
inline void print_warning(Args... args)
{
    (cerr << "WARNING: " <<... << args) << endl;
}



#endif // ERRORS_H