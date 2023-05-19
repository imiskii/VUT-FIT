/**
 * @file ipkcpc.h
 * @author Michal Ľaš
 * @brief header file for ipkcpc.cpp
 * @date 2023-02-22
 * 
 */

#include <iostream>
#include "errors.hpp"
#include "ipkcpc_udp.hpp"
#include "ipkcpc_tcp.hpp"
#include "parseArg.hpp"


#ifndef IPKCPC_H
#define IPKCPC_H

/****************** MACROS ******************/

// Timeout for UDP message in seconds
#define UDP_TIMEOUT 5

/**************** FUNCTIONS *****************/

/**
 * @brief Loop through user input and send given data with UDP protocol to server with given IP on given port
 * and print received data from server.
 * 
 * @param ip ip address of server where message will be send
 * @param port port that will be used to comunicate with server
 */
void use_UDP(std::string ip, int port);


/**
 * @brief Loop through user input and send given data with TCP protocol to server with given IP on given port
 * and print received data from server.
 * 
 * @param ip ip address of server where message will be send
 * @param port port that will be used to comunicate with server
 */
void use_TCP(std::string ip, int port);

#endif // IPKCPC_H