/**
 * @file nscan-tcp.cpp
 * @author Michal Ľaš
 * @brief classes and functions for scanning ports with TCP
 * @date 2023-03-08
 * 
 */


#include "nscan-tcp.hpp"


/*** Functions and methods ***/


TCPScanner::TCPScanner(string interface, string target_ip, string local_ip, int ip_type, int timeout, int socket_fd, iphdr *IPv4Header, ip6_hdr *IPv6Header, tcphdr *TCPHeader)
{
    this->portStatus = "filtered";
    this->__sock = socket_fd;
    this->__interface = interface;
    this->__ipAddress = target_ip;
    this->__localIP = local_ip;
    this->__ipType = ip_type;
    this->__timeout = timeout;
    this->__tcpHeader = TCPHeader;
    this->__IPv4Header = nullptr;
    this->__IPv6Header = nullptr;

    if (this->__ipType == 4) // IPv4
    {
        this->__IPv4Header = IPv4Header;
    }
    else // IPv6
    {
        this->__IPv6Header = IPv6Header;
    }
}


TCPScanner::~TCPScanner()
{
}


unsigned short TCPScanner::__calcTCPHeaderCheckSum()
{
    if (this->__ipType == 4) // IPv4
    {   
        struct tcp4PseudoHeader pseudo_hdr;
        pseudo_hdr.placeholder = 0;
        pseudo_hdr.tcp_length = htons(sizeof(struct tcphdr));
        pseudo_hdr.protocol = IPPROTO_TCP;
        pseudo_hdr.daddr = this->__IPv4Header->daddr;
        pseudo_hdr.saddr = this->__IPv4Header->saddr;
        memcpy(&pseudo_hdr.tcp, this->__tcpHeader, sizeof(struct tcphdr));

        return calcHeaderCheckSum((unsigned short*) &pseudo_hdr, sizeof(struct tcp4PseudoHeader));
    }
    else // IPv6
    {
        struct tcp6PseudoHeader pseudo_hdr;
        memset(&pseudo_hdr.placeholder, 0, sizeof(pseudo_hdr.placeholder));
        pseudo_hdr.tcp_length = htons(sizeof(struct tcphdr));
        pseudo_hdr.protocol = IPPROTO_TCP;
        inet_pton(AF_INET6, this->__ipAddress.c_str(), &pseudo_hdr.daddr);
        inet_pton(AF_INET6, this->__localIP.c_str(), &pseudo_hdr.saddr);
        memcpy(&pseudo_hdr.tcp, this->__tcpHeader, sizeof(struct tcphdr));

        return calcHeaderCheckSum((unsigned short*) &pseudo_hdr, sizeof(struct tcp6PseudoHeader));
    }
}


string TCPScanner::TCPScan(int port)
{
    // Set port that will be sacned to TCP header
    this->__tcpHeader->dest = htons(port);
    // Calculate the TCP checksum
    this->__tcpHeader->check = this->__calcTCPHeaderCheckSum();

    // Calculate IP and TCP headers size
    size_t IPHeaderSize = (this->__ipType == 6) ? sizeof(*this->__IPv6Header) : sizeof(*this->__IPv4Header);
    size_t TCPHeaderSize = sizeof(*this->__tcpHeader);

    // Create a packet
    u_char buffer[IPHeaderSize + TCPHeaderSize];
    // Set it to 0
    memset(&buffer, 0, sizeof(buffer));
    // Set IP header
    (this->__ipType == 6) ? memcpy(buffer, this->__IPv6Header, IPHeaderSize) : memcpy(buffer, this->__IPv4Header, IPHeaderSize);

    // Set TCP header
    memcpy(buffer + IPHeaderSize, this->__tcpHeader, TCPHeaderSize);
    
    // Define destination
    struct sockaddr_in IPv4destAddress;
    struct sockaddr_in6 IPv6destAddress;
    if (this->__ipType == 4) // IPv4
    {
        memset(&IPv4destAddress, 0, sizeof(IPv4destAddress));
        IPv4destAddress.sin_family = AF_INET;
        inet_pton(AF_INET, this->__ipAddress.c_str(), &(IPv4destAddress.sin_addr.s_addr));
        IPv4destAddress.sin_port = htons(port);
    }
    else // IPv6
    {
        memset(&IPv6destAddress, 0, sizeof(IPv6destAddress));
        IPv6destAddress.sin6_family = AF_INET6;
        inet_pton(AF_INET6, this->__ipAddress.c_str(), &(IPv6destAddress.sin6_addr));
        IPv6destAddress.sin6_flowinfo = 0;
        IPv6destAddress.sin6_scope_id = 0;
    }

    // Open interface
    char errorBuffer[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_handle = pcap_open_live(this->__interface.c_str(), MAX_PACKET_SIZE, false, 1000, errorBuffer);
    if (pcap_handle == nullptr)
    {
        perror("ERR: pcap_open_live");
        return "";
    }
    
    // Set filter
    struct bpf_program filter;
    // expression => use tcp with given ip address and port
    // filter: take just ip addresses from target and from scanned port
    string filterExpression = "tcp and src host " + this->__ipAddress + " and src port " + to_string(port);
    if (pcap_compile(pcap_handle, &filter, filterExpression.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        cerr << "ERR: pcap_compile: " << pcap_geterr(pcap_handle) << endl;
        pcap_close(pcap_handle);
        return "";
    }
    
    if (pcap_setfilter(pcap_handle, &filter) == -1)
    {
        cerr << "ERR: pcap_setfilter: " << pcap_geterr(pcap_handle) << endl;
        pcap_close(pcap_handle);
        return "";
    }

    __ProvideHandlerData(pcap_handle, &this->__ipType, &this->portStatus);
    short trys = 2; // 2 trys for sending the packet
    
    while (trys)
    {
        // Send packet
        int bytes = sendto(this->__sock, buffer, IPHeaderSize + TCPHeaderSize, 0, 
                          ((this->__ipType == 6) ? (struct sockaddr*)&IPv6destAddress : (struct sockaddr*)&IPv4destAddress),
                          ((this->__ipType == 6) ? sizeof(IPv6destAddress) : sizeof(IPv4destAddress)));
        if (bytes == -1) {
            perror("ERR: sendto");
            pcap_close(pcap_handle);
            return "";
        }
        
        // Set timeout for packet arriving
        signal(SIGALRM, __packetArriveTimeout);
        struct itimerval timer;
        timer.it_value.tv_sec = this->__timeout/1000;
        timer.it_value.tv_usec = (this->__timeout%1000)*1000;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);

        // Capture packet
        int status = pcap_loop(pcap_handle, 1, __processPacket, nullptr);
        if (status == 0) // Packet was received
        {
            break;
        }
        else if (status = PCAP_ERROR_BREAK) // Timeout try second attempt
        {
            trys--;
            continue;
        }
        else if (status == PCAP_ERROR) // pacp_loop() error
        {
            cerr << "ERR: pcap_loop: " << pcap_geterr(pcap_handle) << endl;
            pcap_close(pcap_handle);
            return "";
        }
    }

    pcap_close(pcap_handle);

    return this->portStatus;
}


void TCPScanner::__processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct TCPHandlerData *HData = __ProvideHandlerData();

    // Find datalink size
    int linkType = pcap_datalink(HData->handler);
    int datalinkSize = 0;
    
    switch (linkType)
    {
    case DLT_EN10MB:
        datalinkSize = 14;
        break;
    case DLT_LINUX_SLL:
        datalinkSize = 16;
    default:
        *HData->portStatus = "unknow";
        return;
    }

    // Parse received packet
    struct tcphdr *TCPHeader;
    uint8_t protocol;
    if (*HData->IP_Type_p == 6)
    {
        struct ip6_hdr *ipHeader = (struct ip6_hdr *)(packet + datalinkSize);
        TCPHeader = (struct tcphdr *)(packet + datalinkSize + 40);   // 40 = size of IPv6 header
        protocol = ipHeader->ip6_nxt;
    }
    else
    {
        struct iphdr *ipHeader = (struct iphdr *)(packet + datalinkSize);
        TCPHeader = (struct tcphdr *)(packet + datalinkSize + ipHeader->ihl*4);
        protocol = ipHeader->protocol;
    }

    // Find result
    if (protocol != IPPROTO_TCP)
    {
        *HData->portStatus = "closed";
        return;
    }

    if (TCPHeader->ack == 1 && TCPHeader->syn == 1)
    {
        *HData->portStatus = "open";
        return;
    }
    else if (TCPHeader->ack == 1 && TCPHeader->rst == 1)
    {
        *HData->portStatus = "closed";
        return;
    }
}


struct TCPHandlerData* TCPScanner::__ProvideHandlerData(pcap_t *handler, int *ip_type_p, string *portStatus)
{
    static struct TCPHandlerData data;

    if (handler == nullptr || ip_type_p == nullptr || portStatus == nullptr)
    {
        return &data;
    }
    else
    {
        data.handler = handler;
        data.IP_Type_p = ip_type_p;
        data.portStatus = portStatus;
    }
    
    return nullptr;
}



void TCPScanner::__packetArriveTimeout(int s)
{
    struct TCPHandlerData *HData = __ProvideHandlerData();
    *HData->portStatus = "filtered";
    pcap_breakloop(HData->handler);
}


/* END OF FILE */