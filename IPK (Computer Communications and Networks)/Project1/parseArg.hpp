/**
 * @file parseArg.hpp
 * @author Michal Ľaš
 * @brief header file for parseArg.cpp
 * @date 2023-03-21
 * 
 */


#include <iostream>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include "errors.hpp"


#ifndef ARG_PARSE_H
#define ARG_PARSE_H


/**************** STRUCTURES ****************/

/**
 * @brief Program arguments: IP address, prot and mode
 * 
 */
struct ProgramArgs
{
    std::string ip_addr;
    int port;
    std::string mode;
};


/**************** FUNCTIONS *****************/

/**
 * @brief Validate given ip address. IP adress has to be version 4 (IPv4)
 * 
 * @param ip IP address that will be validated
 * @return true If this IP address is valid
 * @return false if this IP address is invalid
 */
bool checkIP(std::string ip);


/**
 * @brief Validate given port. Check if given port is in valid port range 1 - 65 536
 * 
 * @param port given port
 */
void checkPort(int port);


/**
 * @brief Function for getting IPv4 address from given hostname
 * Function exit with error code 20 if invalid hostname was given
 * or if any IPv4 address for given hostname was not found
 * 
 * @param host hostname
 * @return std::string IPv4 address
 */
std::string getIPfromHostname(std::string host);


/**
 * @brief Process and validate program arguments.
 * In case that an invalid argument has occurred, function exits with exit code 20 and print error message to STDERR.
 * 
 * @param argc number of program arguments
 * @param argv pointer to array with program arguments
 * @return ProgramArgs struct with parsed program arguments
 */
ProgramArgs ArgParse(int argc, char *argv[]);


#endif //ARG_PARSE_H