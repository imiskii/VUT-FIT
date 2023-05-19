/**
 * @file headers.hpp
 * @author Michal Ľaš
 * @brief header file for headers.cpp
 * @date 2023-03-09
 * 
 */


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>


using namespace std;


#ifndef HEADERS_H
#define HEADERS_H


/******************** MACROS ********************/

#define TCP_P 1     // flag for TCP protocol
#define UDP_P 2     // flag for UDP protocol

#define MAX_PACKET_SIZE 65535


/*********** FUNCTIONS AND CLASSES **************/


/**
 * @brief Function for calculating checksum of IP header \n
 * 
 * Taken from \n
 * Title: How to Calculate IP/TCP/UDP Checksum \n
 * Author: David Hoze \n
 * Date: 2019 \n
 * Code version: ? \n
 * Availability: https://gist.github.com/david-hoze/0c7021434796997a4ca42d7731a7073a
 * 
 * @param addr address of iphdr/ip6_hdr structure
 * @param count size of header in bytes
 * @return unsigned short checksum for header
 */
unsigned short calcHeaderCheckSum(unsigned short *addr, unsigned int count) ;


/**
 * @brief Function creates IPv4 header
 * 
 * @param protocol TCP_P/UDP_P (1/2)
 * @param iphdr pointer to IPv4 header
 * @param targetIPAddress destination ip address
 * @param localIPAddress source ip address
 */
void createIPv4Header(int protocol, struct iphdr *iphdr, string targetIPAddress, string localIPAddress);


/**
 * @brief Function creates IPv6 header
 * 
 * @param protocol TCP_P/UDP_P (1/2)
 * @param iphdr pointer to IPv6 header
 * @param targetIPAddress destination ip address
 * @param localIPAddress source ip address
 */
void createIPv6Header(int protocol, struct ip6_hdr *iphdr, string targetIPAddress, string localIPAddress);


/**
 * @brief Function creates TCP SYN header
 * 
 * @param tcpHeader pointer to TCP header
 * @param src_port source port
 * @param dest_port destination port
 * @param seq sequence number
 */
void createTCPHeader(struct tcphdr *tcpHeader, int src_port, int dest_port, int seq);


/**
 * @brief Function creates UDP header
 * 
 * @param udpHeader pointer to UDP header
 * @param src_port source port
 * @param dest_port destination port
 */
void createUDPHeader(struct udphdr *udpHeader, int src_port, int dest_port);


#endif // HEADERS_H