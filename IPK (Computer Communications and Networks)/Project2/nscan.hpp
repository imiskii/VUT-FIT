/**
 * @file nscan.hpp
 * @author Michal Ľaš
 * @brief header file for nscan.cpp
 * @date 2023-03-05
 * 
 */


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pcap/pcap.h>
#include <ifaddrs.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <random>
#include "errors.hpp"
#include "argparse.hpp"
#include "nscan-tcp.hpp"
#include "nscan-udp.hpp"


#ifndef NSCAN_H
#define NSCAN_H


using namespace std;


/******************** MACROS ********************/

// flag for get socket from getTCPSocket() or getUDPSocket()
#define GET_SOCKET 1

// flag for deleting/reseting socket in getTCPSocket() or getUDPSocket()
#define DEL_SOCKET 0


/*********** FUNCTIONS AND CLASSES **************/


/**
 * @brief Get the Local IP address
 * 
 * @param IPv version of local ip address that should be found
 * @return string ip address of given version or empty string if ip address was not found
 */
string getLocalIP(int IPv);


/**
 * @brief Create/Close/Return socket for sending TCP packets
 * GET_SOCKET - creates new socket or return existing socket
 * DEL_SOCKET - close existing socket or do nothing
 * 
 * function store socket descriptor in static variable
 * 
 * @param mode GET_SOCKET/DEL_SOCKET
 * @param ip_type type of ip address for which will be the socket designated
 * @return int valid socket descriptor or 0 if socket is not created
 */
int getTCPSocket(int mode = 0, int ip_type = 4);


/**
 * @brief Create/Close/Return socket for sending UDP packets
 * GET_SOCKET - creates new socket or return existing socket
 * DEL_SOCKET - close existing socket or do nothing
 * 
 * function store socket descriptor in static variable
 * 
 * @param mode GET_SOCKET/DEL_SOCKET
 * @param ip_type type of ip address for which will be the socket designated
 * @return int valid socket descriptor or 0 if socket is not created
 */
int getUDPSocket(int mode = 0, int ip_type = 4);


/**
 * @brief C-c signal handler
 * 
 * @param signum signal number
 */
void interuptSignalHandler(int signum);


#endif // NSCAN_H