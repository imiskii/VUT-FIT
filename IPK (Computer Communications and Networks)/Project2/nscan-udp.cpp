/**
 * @file nscan-udp.cpp
 * @author Michal Ľaš
 * @brief classes and functions for scanning ports with UDP
 * @date 2023-03-08
 * 
 */


#include "nscan-udp.hpp"


/*** Functions and methods ***/


UDPScanner::UDPScanner(string interface, string target_ip, string local_ip, int ip_type, int timeout, int socket_fd, iphdr *IPv4Header, ip6_hdr *IPv6Header, udphdr *UDPHeader)
{
    this->portStatus = "open";
    this->__sock = socket_fd;
    this->__interface = interface;
    this->__ipAddress = target_ip;
    this->__localIP = local_ip;
    this->__ipType = ip_type;
    this->__timeout = timeout;
    this->__udpHeader = UDPHeader;
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


UDPScanner::~UDPScanner()
{
}


unsigned short UDPScanner::__calcUDPHeaderCheckSum()
{
    if (this->__ipType == 4) // IPv4
    {
        struct udp4PseudoHeader pseudo_hdr;
        pseudo_hdr.placeholder = 0;
        pseudo_hdr.udp_length = htons(sizeof(struct udphdr));
        pseudo_hdr.protocol = IPPROTO_UDP;
        pseudo_hdr.daddr = this->__IPv4Header->daddr;
        pseudo_hdr.saddr = this->__IPv4Header->saddr;
        memcpy(&pseudo_hdr.udp, this->__udpHeader, sizeof(struct udphdr));

        return calcHeaderCheckSum((unsigned short*) &pseudo_hdr, sizeof(struct udp4PseudoHeader));
    }
    else // IPv6
    {
        struct udp6PseudoHeader pseudo_hdr;
        memset(&pseudo_hdr.placeholder, 0, sizeof(pseudo_hdr.placeholder));
        pseudo_hdr.udp_length = htons(sizeof(struct udphdr));
        pseudo_hdr.protocol = IPPROTO_UDP;
        inet_pton(AF_INET6, this->__ipAddress.c_str(), &pseudo_hdr.daddr);
        inet_pton(AF_INET6, this->__localIP.c_str(), &pseudo_hdr.saddr);
        memcpy(&pseudo_hdr.udp, this->__udpHeader, sizeof(struct udphdr));

        return calcHeaderCheckSum((unsigned short*) &pseudo_hdr, sizeof(struct udp6PseudoHeader));
    }
}


string UDPScanner::UDPScan(int port)
{
    // Set port that will be sacned to UDP header
    this->__udpHeader->dest = htons(port);
    // Calculate the UDP checksum
    this->__udpHeader->check = this->__calcUDPHeaderCheckSum();
    
    // Calculate IP and UDP headers size
    size_t IPHeaderSize = (this->__ipType == 6) ? sizeof(*this->__IPv6Header) : sizeof(*this->__IPv4Header);
    size_t UDPHeaderSize = sizeof(*this->__udpHeader);

    // Create a packet
    u_char buffer[IPHeaderSize + UDPHeaderSize];
    // Set it to 0
    memset(&buffer, 0, sizeof(buffer));
    // Set IP header
    (this->__ipType == 6) ? memcpy(buffer, this->__IPv6Header, IPHeaderSize) : memcpy(buffer, this->__IPv4Header, IPHeaderSize);

    // Set UDP header
    memcpy(buffer + IPHeaderSize, this->__udpHeader, UDPHeaderSize);
    
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
    pcap_t *pcap_handle = pcap_open_live(this->__interface.c_str(), MAX_PACKET_SIZE, false, 500, errorBuffer);
    if (pcap_handle == nullptr)
    {
        cerr << "ERR: pcap_open_live: " << pcap_geterr(pcap_handle) << endl;
        return "";
    }
    
    // Set filter
    struct bpf_program filter;
    // expression => use udp with given ip address and port
    // filter: take just ip addresses from target and from scanned port
    // string filterExpression = "udp and src host " + this->__ipAddress + " and src port " + to_string(port);
    string filterExpression = "icmp or icmp6 and src host " + this->__ipAddress;
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
    
    // Send packet
    int bytes = sendto(this->__sock, buffer, IPHeaderSize + UDPHeaderSize, 0, 
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
    if (status == PCAP_ERROR) // pacp_loop() error
    {
        cerr << "ERR: pcap_loop: " << pcap_geterr(pcap_handle) << endl;
        pcap_close(pcap_handle);
        return "";
    }

    pcap_close(pcap_handle);

    return this->portStatus;
}


void UDPScanner::__processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct UDPHandlerData *HData = __ProvideHandlerData();

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
    struct icmp6_hdr *ICMP6_Header;
    struct icmphdr *ICMP4_Header;
    unsigned int protocol;
    if (*HData->IP_Type_p == 6)
    {
        const u_char *iph = packet + datalinkSize;
        struct ip6_hdr *ipHeader = (struct ip6_hdr *)(iph);
        const u_char *icmph = packet + datalinkSize + sizeof(struct ip6_hdr);
        ICMP6_Header = (struct icmp6_hdr *)(icmph);   
        protocol = (unsigned int)ipHeader->ip6_nxt;
    }
    else
    {
        const u_char *iph = packet + datalinkSize;
        struct iphdr *ipHeader = (struct iphdr *)(iph);
        const u_char *icmph = packet + datalinkSize + ipHeader->ihl*4;
        ICMP4_Header = (struct icmphdr *)(icmph);
        protocol = (unsigned int)ipHeader->protocol;
    }

    // Find result
    if ((protocol != IPPROTO_ICMP) && (protocol != IPPROTO_ICMPV6))
    {
        *HData->portStatus = "open";
        return;
    }

    if (*HData->IP_Type_p == 6)
    {
        if((unsigned int)ICMP6_Header->icmp6_type == 1 && (unsigned int)ICMP6_Header->icmp6_code == 4)
        {
            *HData->portStatus = "closed";
            return;
        }
    }
    else
    {
        if(ICMP4_Header->type == 3 && ICMP4_Header->code == 3)
        {
            *HData->portStatus = "closed";
            return;
        }
    }
}


struct UDPHandlerData* UDPScanner::__ProvideHandlerData(pcap_t *handler, int *ip_type_p, string *portStatus)
{
    static struct UDPHandlerData data;

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


void UDPScanner::__packetArriveTimeout(int s)
{
    struct UDPHandlerData *HData = __ProvideHandlerData();
    *HData->portStatus = "open";
    pcap_breakloop(HData->handler);
}


/* END OF FILE */