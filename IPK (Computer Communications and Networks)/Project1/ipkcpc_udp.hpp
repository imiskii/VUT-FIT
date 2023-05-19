/**
 * @file ipkcpc_udp.hpp
 * @author Michal Ľaš xlasmi00
 * @brief header file for ipkcpc_udp.cpp
 * @date 2023-03-21
 * 
 */


#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include "errors.hpp"


#ifndef UDP_H
#define UDP_H


/****************** MACROS ******************/

#define _XOPEN_SOURCE_EXTENDED 1

// Flag for reset socekt
#define DEL_SOCK 0
// Flag for return existing socket descriptor
#define GET_SOCK 1

// Recive message buffer size
#define BUFFER_SIZE 4096

/**************** FUNCTIONS *****************/

/**
 * @brief Add to given data the IPKCP header
 * 
 * @param data data to which the IPKCP header will be added
 * @param msg pointer to new created message with IPKCP header
 */
void IPKCPmsgEncode(std::string data, unsigned char *msg);


/**
 * @brief Decode and validate received message using IPKCP protocol
 * 
 * @param data data that will be decoded
 * @param output reference to variable where final output will be placed
 * @return true If decoded message is valid
 * @return false if decoded message is invalid
 */
bool IPKCPmsgDecode(char *data);


/**
 * @brief Close given UDP socket descriptor
 * 
 * @param client_socket socket desriptor
 */
void closeSocketUDP(int &client_socket);


/**
 * @brief Handle program interuption with C-c while using UDP protocol.
 * Function write message to STDERR, close socket descriptor and exit the program.
 * 
 * @param signum signal number
 */
void UDPSignalHandler(int signum);


/**
 * @brief Create and return new socket descriptor or return existing socket descriptor for UDP connection.
 * This function keep the socket descriptor in static variable. 
 * If flag is set to 0 function set socket descriptor to 0 
 * else function return new or existing socket descriptor
 * 
 * @param flag flag
 * @return int Socket descriptor
 */
int getUDPSocket(int flag);


/**
 * @brief Encode given message with IPKCP protocol and send this message
 * using UDP protocol to given IP address on given port. Then the function waits on response from the server
 * and return recived data.
 * 
 * In case of error while sending or receiving error message from the server function print warning message to STDERR and contiunue loading input from STDIN.
 * 
 * @param server_address structure describing internet socket address
 * @param client_socket socket descriptor
 * @param msg message to send to server
 * @return message received from server
 */
std::string IPKCP_UDP(sockaddr_in server_address, int client_socket, std::string msg);


#endif // UDP_H