/**
 * @file nscan-tcp.hpp
 * @author Michal Ľaš
 * @brief header file for nscan-udp.cpp
 * @date 2023-03-08
 * 
 */


#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <pcap/pcap.h>
#include "headers.hpp"
#include "errors.hpp"


#ifndef NSCAN_TCP
#define NSCAN_TCP

/****************** DATATYPES *******************/

/**
 * @brief Pseudo TCP header for calculating TCP checksum (only for IPv4 TCP packets)
 * 
 */
struct tcp4PseudoHeader
{
    uint32_t saddr;          // source address
	uint32_t daddr;          // destination address
	uint8_t placeholder;     // placeholder
	uint8_t protocol;        // protocol
	uint16_t tcp_length;     // tcp length
	
	struct tcphdr tcp;       // tcp header
};


/**
 * @brief Pseudo TCP header for calculating TCP checksum (only for IPv6 TCP packets)
 * 
 */
struct tcp6PseudoHeader
{
    uint8_t saddr[16];       // source address
    uint8_t daddr[16];       // destination address
    uint8_t placeholder[3];  // placeholder
    uint8_t protocol;        // protocol
    uint32_t tcp_length;     // tcp length

    struct tcphdr tcp;       // tcp header
};


/**
 * @brief Data necessary to process packet
 * 
 */
struct TCPHandlerData
{
    pcap_t *handler;            // pacp handler
    int *IP_Type_p;             // pointer to type of IP address
    string *portStatus;         // pointer to result of port scanning
};


/*********** FUNCTIONS AND CLASSES **************/


/**
 * @brief Class for TCP SYN scanning
 * 
 * Attributes:
 * ---------------
 * struct iphdr *__IPv4Header: pointer IPv4 header
 * struct ip6_hdr *__IPv6Header: pointer IPv6 header
 * struct tcphdr *__tcpHeader: pointer TCP header
 * string __interface: interface that is used
 * string __ipAddress: destination ip address/target ip address
 * int __ipType: type of ip address (IPv4/IPv6)
 * int __timeout: maximum waiting time for packet receiving
 * string __localIP: source ip address/local ip address
 * int __sock: socket descriptor
 * string portStatus: result of port scanning
 * 
 * Methods:
 * ---------------
 * unsigned short __calcTCPHeaderCheckSum()
 * static void __processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
 * static struct TCPHandlerData *__ProvideHandlerData(pcap_t *handler = nullptr, int *ip_type_p = nullptr, string *portStatus = nullptr)
 * static void __packetArriveTimeout(int s)
 * string TCPScan(int port)
 * 
 */
class TCPScanner
{
private:
    // Private Attributes
    struct iphdr *__IPv4Header;
    struct ip6_hdr *__IPv6Header;
    struct tcphdr *__tcpHeader;
    string __interface;
    string __ipAddress;
    int __ipType;
    int __timeout;
    string __localIP;
    int __sock;
    // Private Methods

    /**
     * @brief Calculate checksum for TCP header
     * 
     * @return unsigned checksum value
     */
    unsigned short __calcTCPHeaderCheckSum();


    /**
     * @brief process received packet
     * 
     * @param args 
     * @param header 
     * @param packet received packet
     */
    static void __processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

    /**
     * @brief Function for manging data for port scanning
     * If one of the function parameters is not provided function return struct TCPHandlerData that contains the data
     * If every parameter is provided function saves the data and return nullptr
     * 
     * @param handler pointer to pcap handler
     * @param ip_type_p pointer to ip version type
     * @param portStatus pointer result of port scanning
     * @return struct TCPHandlerData* pointer to struct with data or nullptr
     */
    static struct TCPHandlerData *__ProvideHandlerData(pcap_t *handler = nullptr, int *ip_type_p = nullptr, string *portStatus = nullptr);

    /**
     * @brief Method for handeling timeout for packet receiving
     * 
     * @param s signal numer
     */
    static void __packetArriveTimeout(int s);
public:
    // Public Attributes
    string portStatus;
    // Public Methods

    /**
     * @brief Construct a new TCPScanner object
     * 
     * @param interface interface that will be used
     * @param target_ip destination ip address
     * @param local_ip source ip address
     * @param ip_type version of ip address
     * @param timeout maximum waiting time for packet receiving
     * @param socket_fd socket descriptor
     * @param IPv4Header pointer to IPv4 header
     * @param IPv6Header pointer to IPv4 header
     * @param TCPHeader pointer to TCP header
     */
    TCPScanner(string interface, string target_ip, string local_ip, int ip_type, int timeout, int socket_fd,iphdr *IPv4Header, ip6_hdr *IPv6Header, tcphdr *TCPHeader);
    ~TCPScanner();

    /**
     * @brief Function scan given port (TCP SYN scanning)
     * 
     * @param port port that will be scanned
     * @return string result of port scanning
     */
    string TCPScan(int port);
};


#endif // NSCAN_TCP