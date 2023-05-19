/**
 * @file headers.cpp
 * @author Michal Ľaš
 * @brief functions for creating IP, TCP and UDP headers
 * @date 2023-03-09
 * 
 */


#include "headers.hpp"


using namespace std;



unsigned short calcHeaderCheckSum(unsigned short *addr, unsigned int count) 
{
    unsigned long sum = 0;
    while (count > 1) {
        sum += * addr++;
        count -= 2;
    }
    //if any bytes left, pad the bytes and add
    if(count > 0) {
        sum += ((*addr)&htons(0xFF00));
    }
    //Fold sum to 16 bits: add carrier to result
    while (sum>>16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    //one's complement
    sum = ~sum;
    return ((unsigned short)sum);
}


void createIPv4Header(int protocol, struct iphdr *iphdr, string targetIPAddress, string localIPAddress)
{
    memset(iphdr, 0, sizeof(struct iphdr));
    iphdr->version = 4;                                 // version number of IP
    iphdr->ihl = 5;                                     // length of the IP header (no options so set it to 5 => 20 bytes)
    iphdr->tos = 0;                                     // type of service
    iphdr->id = htons(0);                               // identification number
    iphdr->frag_off = htons(IP_DF);                     // Fragment offset
    iphdr->ttl = 255;                                   // time to live
    iphdr->saddr = inet_addr(localIPAddress.c_str());   // source address
    iphdr->daddr = inet_addr(targetIPAddress.c_str());  // destination address
    iphdr->check = 0;                                   // control checksum

    if (protocol == TCP_P)
    {
        iphdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));   // total len of IP datagram (header+data)
        iphdr->protocol = IPPROTO_TCP;                                          // protocol field (identify next layer protocol)
    }
    else
    {
        iphdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr));   // total len of IP datagram (header+data)
        iphdr->protocol = IPPROTO_UDP;                                          // protocol field (identify next layer protocol)
    }

    iphdr->check = calcHeaderCheckSum((unsigned short*)iphdr, iphdr->ihl<<2);
}


void createIPv6Header(int protocol, struct ip6_hdr *iphdr, string targetIPAddress, string localIPAddress)
{
    memset(iphdr, 0, sizeof(struct ip6_hdr));
    iphdr->ip6_flow = htonl(6 << 28);                   // version number of IP and set remaining 28 bits to 0 (traffic class and flow label fields will both be 0, which is the default value for these fields)
    iphdr->ip6_hops = 255;                              // hop limit (equivalent of TTL)
    inet_pton(AF_INET6, localIPAddress.c_str(), &iphdr->ip6_src);   // Set the source IPv6 address
    inet_pton(AF_INET6, targetIPAddress.c_str(), &iphdr->ip6_dst);  // Set the destination IPv6 address

    if (protocol == TCP_P)
    {
        iphdr->ip6_plen = htons(sizeof(struct tcphdr)); // payload length of the IPv6 packet (this is just payload so it do not include ip header lenght like in IPv4 header)
        iphdr->ip6_nxt = IPPROTO_TCP;                   // protocol number of next header
    }
    else
    {
        iphdr->ip6_plen = htons(sizeof(struct udphdr)); // payload length of the IPv6 packet (this is just payload so it do not include ip header lenght like in IPv4 header)
        iphdr->ip6_nxt = IPPROTO_UDP;                   // protocol number of next header
    }
}


void createTCPHeader(struct tcphdr *tcpHeader, int src_port, int dest_port, int seq)
{
    memset(tcpHeader, 0, sizeof(*tcpHeader));
    tcpHeader->source = htons(src_port);          // Set random source port
    tcpHeader->dest = htons(dest_port);           // Set the destination port
    tcpHeader->seq = htonl(seq);                  // Set the sequence number
    tcpHeader->ack_seq = 0;                       // Acknowledgment number
    tcpHeader->doff = sizeof(struct tcphdr) / 4;  // Length of the TCP header in 32-bit words (5 => 20 bytes like for IP header)
    tcpHeader->window = htons(0);                 // Set max win size (just something small, there wont be any sended data)
    tcpHeader->check = 0;                         // control checksum (set it to 0 before calculation actuall checksum)

    tcpHeader->syn = 1;                           // Send SYN packet so SYN set to 1
    tcpHeader->fin = 0;                           // other flags set to 0
    tcpHeader->rst = 0;
    tcpHeader->psh = 0;
    tcpHeader->ack = 0;
    tcpHeader->urg = 0;
    tcpHeader->urg_ptr = 0;
    tcpHeader->res1 = 0;
    tcpHeader->res2 = 0;
}


void createUDPHeader(struct udphdr *udpHeader, int src_port, int dest_port)
{
    memset(udpHeader, 0, sizeof(*udpHeader));
    udpHeader->len = htons(sizeof(struct udphdr));   // Length of the UDP header
    udpHeader->source = htons(src_port);             // Source port
    udpHeader->dest = htons(dest_port);              // Destination port
    udpHeader->check = 0;                            // Checksum (set to 0 before calculation)
}