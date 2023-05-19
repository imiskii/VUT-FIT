/**
 * @file ipkcpc_tcp.hpp
 * @author Michal Ľaš
 * @brief header file for ipkcpc_tcp.cpp
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


#ifndef TCP_H
#define TCP_H


/****************** MACROS ******************/

#define _XOPEN_SOURCE_EXTENDED 1

// Flag for reset socekt
#define DEL_SOCK 0
// Flag for return existing socket descriptor
#define GET_SOCK 1

// Maximum trys for saying BYE to server
#define BYE_TRYS 5

// Recive message buffer size
#define BUFFER_SIZE 4096

/**************** FUNCTIONS *****************/


/**
 * @brief Close given TCP socket descriptor
 * 
 * @param client_socket socket desriptor
 */
void closeSocketTCP(int &client_socket);


/**
 * @brief Function try to send 5 times BYE message to server if it does not success then exit with error code 22
 * Function is used to close connection after getting C-c signal or after end of program input
 * 
 */
void closeTCPConnection();


/**
 * @brief Handle program interuption with C-c while using TCP protocol.
 * Function write message to STDERR, send BYE message to server, close socket descriptor and exit the program.
 * 
 * @param signum signal number
 */
void TCPSignalHandler(int signum);


/**
 * @brief Create and return new socket descriptor or return existing socket descriptor for TCP connection.
 * This function keep the socket descriptor in static variable. 
 * If flag is set to 0 function set socket descriptor to 0 
 * else function return new or existing socket descriptor
 * 
 * @param flag flag
 * @return int Socket descriptor
 */
int getTCPSocket(int flag);


/**
 * @brief Function sends given message to the server with given socekt descriptor. Function waits for response and return
 * recived data.
 * 
 * In case of connection error function exit with exit code 1 and print message to STDERR.
 * In case of error while sending or receiving error message from the server function print warning message to STDERR and contiunue loading input from STDIN.
 * 
 * @param client_socket socket descriptor
 * @param msg message to send to server
 * @return message received from server
 */
std::string IPKCP_TCP(int client_socket, std::string msg);


#endif // TCP_H