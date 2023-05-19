/**
 * @file nscan-udp.hpp
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
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <pcap/pcap.h>
#include "headers.hpp"
#include "errors.hpp"


#ifndef NSCAN_UDP
#define NSCAN_UDP

/****************** DATATYPES *******************/


/**
 * @brief Pseudo UDP header for calculating UDP checksum (only for IPv4 UDP packets)
 * 
 */
struct udp4PseudoHeader
{
    uint32_t saddr;         // source address
	uint32_t daddr;         // destination address
	uint8_t placeholder;    // placeholder
	uint8_t protocol;       // protocol
	uint16_t udp_length;    // udp length
	
	struct udphdr udp;      // udp header
};


/**
 * @brief Pseudo UDP header for calculating UDP checksum (only for IPv6 UDP packets)
 * 
 */
struct udp6PseudoHeader
{
    uint8_t saddr[16];      // source address
    uint8_t daddr[16];      // destination address
    uint8_t placeholder[3]; // placeholder
    uint8_t protocol;       // protocol
    uint32_t udp_length;    // udp length

    struct udphdr udp;      // udp header
};


/**
 * @brief Data necessary to process packet
 * 
 */
struct UDPHandlerData
{
    pcap_t *handler;            // pacp handler
    int *IP_Type_p;             // pointer to type of IP address
    string *portStatus;         // pointer to result of port scanning
};

/*********** FUNCTIONS AND CLASSES **************/


/**
 * @brief Class for UDP scanning
 * 
 * Attributes:
 * ---------------
 * struct iphdr *__IPv4Header: pointer IPv4 header
 * struct ip6_hdr *__IPv6Header: pointer IPv6 header
 * struct udphdr *__udpHeader: pointer to UDP header
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
 * unsigned short __calcUDPHeaderCheckSum()
 * static void __processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
 * static void __packetArriveTimeout(int s);
 * UDPScanner(string interface, string target_ip, string local_ip, int ip_type, int timeout, int socket_fd, iphdr *IPv4Header, ip6_hdr *IPv6Header, udphdr *UDPHeader)
 * string UDPScan(int port);
 * 
 */
class UDPScanner
{
private:
    // Private Attributes
    struct iphdr *__IPv4Header;
    struct ip6_hdr *__IPv6Header;
    struct udphdr *__udpHeader;
    string __interface;
    string __ipAddress;
    int __ipType;
    int __timeout;
    string __localIP;
    int __sock;
    // Private Methods

    /**
     * @brief Calculate checksum for UDP header
     * 
     * @return unsigned checksum value
     */
    unsigned short __calcUDPHeaderCheckSum();
    
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
    static struct UDPHandlerData *__ProvideHandlerData(pcap_t *handler = nullptr, int *ip_type_p = nullptr, string *portStatus = nullptr);

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
     * @brief Construct a new UDPScanner object
     * 
     * @param interface interface that will be used
     * @param target_ip destination ip address
     * @param local_ip source ip address
     * @param ip_type version of ip address
     * @param timeout maximum waiting time for packet receiving
     * @param socket_fd socket descriptor
     * @param IPv4Header pointer to IPv4 header
     * @param IPv6Header pointer to IPv4 header
     * @param UDPHeader pointer to TCP header
     */
    UDPScanner(string interface, string target_ip, string local_ip, int ip_type, int timeout, int socket_fd, iphdr *IPv4Header, ip6_hdr *IPv6Header, udphdr *UDPHeader);
    ~UDPScanner();

    /**
     * @brief Function scan given port (UDP scanning)
     * 
     * @param port port that will be scanned
     * @return string result of port scanning
     */
    string UDPScan(int port);
};


#endif // NSCAN_UDP