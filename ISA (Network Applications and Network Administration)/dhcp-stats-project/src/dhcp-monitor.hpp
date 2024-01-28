/**
 * @file dhcp-monitor.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Header file for dhcp-monitor.cpp
 * @date 2023-10-21
 * 
 */


#include <string>
#include <vector>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iostream>
#include <pcap/pcap.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <ctime>
#include <condition_variable>

#include "ip-prefix.hpp"
#include "consol-log.hpp"
#include "errors.hpp"


#ifndef DHCP_MONITOR_H
#define DHCP_MONITOR_H


using namespace std;


/******************** MACROS ********************/

#define DHCP_OPTION_END 255                     /// code for DHCP option that indicates END of DHCP packet options
#define DHCP_OPTION_MSG_TYPE 53                 /// code for DHCP option that indicates Message Type of DHCP packet
#define DHCP_LEASE_TIME_OPT 51                  /// code for DHCP option that indicates Lease Time of DHCP packet
#define DEFAULT_LEASE_TIME_CHECK_FREQUENCY 60   /// default interval in seconds for lease time checks


/****************** DATATYPES *******************/

/**
 * @brief Structure representing DHCP packet (without options)
 * 
 */
struct dhcp_hdr{
    uint8_t op;         /// Operation code
    uint8_t htype;      /// Hardware type
    uint8_t hlen;       /// Hardware address length
    uint8_t hops;       /// Hops
    uint32_t xid;       /// Transaction ID
    uint16_t secs;      /// Seconds elapsed
    uint16_t flags;     /// Flags 
    uint32_t ciaddr;    /// Client IP address
    uint32_t yiaddr;    /// Your IP address
    uint32_t siaddr;    /// Server IP address
    uint32_t giaddr;    /// Relay agent IP address
    uint8_t chaddr[16]; /// Client Hardware Address (MAC address)
    uint8_t sname[64];  /// Optional server host name
    uint8_t file[128];  /// Boot file name
    uint32_t magic_cookie; /// Magic Cookie (Magic Cookie is part of the options in DHCP packet, but due to RFC it is first 4 octets so it is possible to represent like this)
    // + options (options do not have fixed size)
};


/**
 * @brief Structure representing DHCP packet with header and options
 * 
 */
struct dhcp_packet{
    const dhcp_hdr *hdr;       /// pointer to DHCP header
    const u_char *options;     /// pointer where options starts
    size_t opt_len;            /// Options length
};

/*********** FUNCTIONS AND CLASSES **************/


class DHCPMonitor
{
private:
    /* Private Attributes */
    string _interface;                  /// name of interface where monitor listen for packets
    string _pcapFile;                   /// name of pcap file that will be analyzed
    time_t _leaseTimeCheckFrequency;    /// frequency of lease time check in seconds
    pcap_t *_pcapHandle;                /// pcap handler
    struct bpf_program _fp;             /// pcap compile filter
    vector<IPv4Prefix*> _prefixes;      /// monitored ip prefixes
    map<uint32_t, pair<time_t, vector<IPv4Prefix*>>> *_allocatedIPs; /// std::map of ip addresses: <lease time, [list of prefixes including this ip address]>
    mutex _allocatedIPsMapMutex;        /// mutex for access to _allocatedIPs
    bool _networkAnalysisFlag;          /// flag indicating if program is monitoring dhcp traffic on given network interface
    bool _consolFlag;                   /// flag indicating if consol is on/off (true/false)
    bool _logFlag;                      /// flag indicating if syslogs should be made
    atomic<bool> _stopFlag;             /// flag indicating stop of the program
    condition_variable _stopC;          /// condition variable to stop lease time checking
    ConsolLog cl;                       /// command line console

    /* Private Methods */

    /**
     * @brief adds ip address with coresponding datas to _allocatedIPs
     * 
     * @param addr ip address
     * @param leaseTime lease time of this address (0 means infinite lease time)
     */
    void _addIPaddr(uint32_t addr, time_t leaseTime);

    /**
     * @brief removes ip address from _allocatedIPs
     * 
     * @param addr ip address
     */
    void _removeIPaddr(uint32_t addr);

    /**
     * @brief handler for pcap_loop
     * 
     * @param args arguments of method
     * @param header pcap header
     * @param packet captured packet
     */
    static void _packetHandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

    /**
     * @brief proccess DHCP packet for purpose of this program
     * 
     * @param packet dhcp packet
     * @param packetTS time stamp of captured packet
     */
    void _proccessDhcpPacket(dhcp_packet &packet, time_t packetTS);

    /**
     * @brief set pcap filters for capturing packets
     * 
     */
    void _setPcapFilters();

    /**
     * @brief method calling pcap_loop() -> method for capturing packets
     * it is static, bcs. it is used in separate thread
     * 
     * @param monitor pointer to DHCPMonitor (this)
     */
    static void _capturePackets(DHCPMonitor *monitor);

    /**
     * @brief method check lease time for all allocated ip addreses and erase those with expired lease time
     * 
     * this method is not locking mutex for access to _allocatedIPs map, it is bcs. it is only used for lease time check after pcap file processing
     * (in this situation thre are no parallel threds so no mutex is needed)
     */
    void _leaseTimeCheck();

    /**
     * @brief method for periodicly checking lease time of received ip addresses
     * it is static, bcs. it is used in separate thread
     * 
     * @param monitor pointer to DHCPMonitor (this)
     */
    static void _leaseTimeChecker(DHCPMonitor *monitor);

public:
    /* Public Attributes */


    /* Public Methods */

    /**
     * @brief Construct a new DHCPMonitor object
     * 
     * @param prefixes prefixes that will be monitored
     * @param interface name of interface where monitor listen for packets
     * @param pcapFile name of pcap file that will be analyzed
     * @param leaseTimeCheckFrequency frequency of lease time check in seconds
     */
    DHCPMonitor(vector<IPv4Prefix*> prefixes, string interface = "", string pcapFile = "", time_t leaseTimeCheckFrequency = DEFAULT_LEASE_TIME_CHECK_FREQUENCY);

    /**
     * @brief Destroy the DHCPMonitor object
     * 
     */
    ~DHCPMonitor();

    /**
     * @brief Analyze pcap file
     * 
     * @param print flag indicating if after successful analysis the result should be printed (default is true)
     */
    void analyzePcapFile(bool print = true);

    /**
     * @brief Starts monitoring network traffic on given interface, analyze packets, parse given packets, etc.
     * 
     */
    void startMonitoring();

    /**
     * @brief Based on the data provided during the construction of the class instance, 
     * this method analyzes the data from the provided pcap file and starts monitoring network traffic on given interface
     * 
     */
    void analyze();

    /**
     * @brief parse packet with ethernet + ip + udp + dhcp header
     * 
     * @param packet received packet
     * @param len length of packet
     * @return dhcp_packet structure containing only dhcp header and options
     */
    dhcp_packet parseDhcpPacket(const u_char* packet, uint32_t len);
};

#endif // DHCP_MONITOR_H