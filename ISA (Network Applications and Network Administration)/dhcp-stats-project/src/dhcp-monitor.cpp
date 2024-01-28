/**
 * @file dhcp-monitor.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Class for minitoring DHCP traffic on network and analyzing DHCP traffic from pcap file
 * @date 2023-10-21
 * 
 */


#include "dhcp-monitor.hpp"

using namespace std;



DHCPMonitor::DHCPMonitor(vector<IPv4Prefix*> prefixes, string interface, string pcapFile, time_t leaseTimeCheckFrequency) 
: _interface(interface), _pcapFile(pcapFile), _prefixes(prefixes), _leaseTimeCheckFrequency(leaseTimeCheckFrequency), _stopFlag(false), cl(_prefixes, &_stopFlag)
{
    this->_consolFlag = false;
    this->_networkAnalysisFlag = false;
    this->_pcapHandle = nullptr;
    this->_allocatedIPs = new map<uint32_t, pair<time_t, vector<IPv4Prefix*>>>();
    // If interface is given the program also check leas time in dhcp packets
    if (!this->_interface.empty())
    {
        this->_logFlag = true;
    }
    else
    {
        this->_logFlag = false;
    }
}


DHCPMonitor::~DHCPMonitor()
{
    delete this->_allocatedIPs;
}


dhcp_packet DHCPMonitor::parseDhcpPacket(const u_char *packet, uint32_t len)
{
    size_t ethernetHeaderSize = sizeof(struct ether_header);
    size_t ipHeaderSize = sizeof(struct ip);
    size_t udpHeaderSize = sizeof(udphdr);

    dhcp_packet dhcpPacket;
    size_t dhcpPcaketSize = ethernetHeaderSize + ipHeaderSize + udpHeaderSize + sizeof(struct dhcp_hdr);
    if (len <= dhcpPcaketSize)
    {
        dhcpPacket.hdr = nullptr;
        return dhcpPacket;
    }

    // get ip header
    const ip *ipHeader = (struct ip*)(packet + ethernetHeaderSize);
    ipHeaderSize = ipHeader->ip_hl * 4;
    // get udp header
    const udphdr *udpHeader = (struct udphdr*)(packet + ethernetHeaderSize + ipHeaderSize);
    // get dhcp header and options
    const u_char *dhcpPayload = packet + ethernetHeaderSize + ipHeaderSize + udpHeaderSize;
    const dhcp_hdr *dhcpHeader = (struct dhcp_hdr*)dhcpPayload;
    const u_char *dhcpOptions = dhcpPayload + sizeof(struct dhcp_hdr);
    // make dhcp packet
    dhcpPcaketSize = ethernetHeaderSize + ipHeaderSize + udpHeaderSize + sizeof(struct dhcp_hdr);
    dhcpPacket.hdr = dhcpHeader;
    dhcpPacket.options = dhcpOptions;
    dhcpPacket.opt_len = len - dhcpPcaketSize;
    return dhcpPacket;
}


void DHCPMonitor::_proccessDhcpPacket(dhcp_packet &packet, time_t packetTS)
{
    bool addFlag = false;
    bool leasTimeFlag = false;
    time_t leaseTime = 0;
    const u_char *optionPtr = packet.options;
    // iterate trough options
    while (*optionPtr != DHCP_OPTION_END)
    {
        // If option is DHCP Message Type Option (53) AND Length is 1
        if ((*optionPtr == DHCP_OPTION_MSG_TYPE) && (*(optionPtr+1) == 1))
        {
            u_char msgType = *(optionPtr+2);
            switch (msgType)
            {
            case 7: // DHCPRELEASE
                this->_removeIPaddr(ntohl(packet.hdr->ciaddr));
                break;
            case 5: // DHCPACK
                addFlag = true;
                break;
            default:
                break;
            }
        }
        // if option is DHCP Lease Time Option (51) and Length is 4
        else if ((*optionPtr == DHCP_LEASE_TIME_OPT) && (*(optionPtr+1) == 4))
        {
            leasTimeFlag = true;
            uint32_t *leaseTimePtr = (uint32_t *)(optionPtr+2);
            leaseTime = static_cast<time_t>(htonl(*leaseTimePtr));
        }
        optionPtr += (*(optionPtr+1)+2); // Option length + 2 (for option code and length fields)
    }

    // If DHCP message type is DHCPACK and lease time was given add new allocated IP address
    if (addFlag && leasTimeFlag)
    {
        // if program is monitoring traffic on network interface check lease time of packets
        if (this->_networkAnalysisFlag)
        {
            time_t currTime = time(nullptr);
            if ((packetTS + leaseTime) <= currTime)
            {
                return;
            }
        }
        this->_addIPaddr(ntohl(packet.hdr->yiaddr), packetTS + leaseTime);
    }
}


void DHCPMonitor::_packetHandler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    DHCPMonitor *monitor = reinterpret_cast<DHCPMonitor *>(args);
    dhcp_packet dhcpPacket =  monitor->parseDhcpPacket(packet, header->len);
    if (dhcpPacket.hdr != nullptr)
    {
        monitor->_proccessDhcpPacket(dhcpPacket, header->ts.tv_sec);
    }
}


void DHCPMonitor::_capturePackets(DHCPMonitor *monitor)
{
    // Cupture infinite number of packets
    int status = pcap_loop(monitor->_pcapHandle, -1, _packetHandler, reinterpret_cast<u_char *>(monitor));

    // Handle error code
    if (status == -1)
    {
        exit_error(ErrorCodes::PCAP_LOOP_ERR, "pcap_loop(): ", pcap_geterr(monitor->_pcapHandle));
    }
}


void DHCPMonitor::_setPcapFilters()
{
    // Check if pcap handler is created
    if (this->_pcapHandle == nullptr)
    {
        return;
    }

    char errBuf[PCAP_ERRBUF_SIZE];

    // Compile filter
    char filter[] = "udp port 67 and udp port 68";
    if (pcap_compile(this->_pcapHandle, &this->_fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        pcap_close(this->_pcapHandle);
        exit_error(ErrorCodes::PCAP_COMPILE_ERR, "compiling filter (pcap_compile): ", errBuf);
    }

    // Set filter
    if (pcap_setfilter(this->_pcapHandle, &this->_fp) == -1)
    {
        pcap_freecode(&(this->_fp));
        pcap_close(this->_pcapHandle);
        exit_error(ErrorCodes::PCAP_SETFILTER_ERR, "setting filter (pcap_setfilter): ", errBuf);
    }
}


void DHCPMonitor::analyzePcapFile(bool print)
{
    if (this->_pcapFile.empty())
    {
        return;
    }

    // Open pcap offline reader
    char errBuf[PCAP_ERRBUF_SIZE];
    this->_pcapHandle = pcap_open_offline(this->_pcapFile.c_str(), errBuf);
    if (this->_pcapHandle == nullptr)
    {
        exit_error(ErrorCodes::OPEN_PCAP_ERR, "opening pcap file (pcap_open_offline): ", errBuf);
    }

    this->_setPcapFilters();

    // Loop through packets in the pcap file
    struct pcap_pkthdr header;
    const u_char* packet;
    while ((packet = pcap_next(this->_pcapHandle, &header)) != nullptr)
    {
        dhcp_packet dhcpPacket = this->parseDhcpPacket(packet, header.len);
        if (dhcpPacket.hdr != nullptr)
        {
            this->_proccessDhcpPacket(dhcpPacket, header.ts.tv_sec);
        }
    }
    
    // Close pcap handle
    pcap_freecode(&(this->_fp));
    pcap_close(this->_pcapHandle);

    // Print stats
    if (print)
    {
        this->cl.printStats();
    }
}


void DHCPMonitor::startMonitoring()
{
    this->_networkAnalysisFlag = true;
    // Open pcap offline reader
    char errBuf[PCAP_ERRBUF_SIZE];

    this->_pcapHandle = pcap_open_live(this->_interface.c_str(), BUFSIZ, false, 500, errBuf);
    if (this->_pcapHandle == nullptr)
    {
        exit_error(ErrorCodes::OPEN_PCAP_ERR, "opening pcap interface (pcap_open_online): ", errBuf);
    }

    this->_setPcapFilters();
    
    // Open capture packet thread
    thread capturePacketThread(_capturePackets, this);

    // wait until user closes the app
    while (!this->_stopFlag)
    {
        continue;
    }

    // Stop capturing packets
    pcap_breakloop(this->_pcapHandle);
    capturePacketThread.join();

    // Close pcap handle
    pcap_freecode(&(this->_fp));
    pcap_close(this->_pcapHandle);
}


void DHCPMonitor::analyze()
{
    // analyze pcap file
    if (!this->_pcapFile.empty())
    {
        this->analyzePcapFile(false);
    }
    // open consol and start monitoring dhcp traffic
    if (!this->_interface.empty())
    {
        // start thread for consol
        thread consolThred(this->cl.makeConsol, &(this->cl));
        this->_consolFlag = true;

        // start thread for lease time checking
        thread leaseTimeCheckThread(_leaseTimeChecker, this);

        // start network monitoring
        this->startMonitoring();

        // Close threads
        this->_stopC.notify_all();
        leaseTimeCheckThread.join();
        consolThred.join();
    }
    else
    {
        this->cl.printStats();
    }
}


void DHCPMonitor::_addIPaddr(uint32_t addr, time_t leaseTime)
{
    lock_guard<mutex> lock(this->_allocatedIPsMapMutex);
    // Already allocated address
    if (this->_allocatedIPs->count(addr))
    {
        // Update lease time
        this->_allocatedIPs->at(addr).first= leaseTime;
    }
    // New address
    else
    {
        // Insert new address
        vector<IPv4Prefix*> ranges;
        for (auto &prefix : this->_prefixes)
        {
            if (prefix->isInPrefix(addr))
            {
                prefix->addAddress(addr);
                ranges.push_back(prefix);

                // Update consol
                if(this->_consolFlag)
                {
                    this->cl.updateConsol(prefix->getPrefixName());
                }
                // Make log
                if(this->_logFlag)
                {
                    this->cl.makeSysLog(prefix->getPrefixName());
                }
            }
        }
        this->_allocatedIPs->insert(make_pair(addr, make_pair(leaseTime, ranges)));
    }
}


void DHCPMonitor::_removeIPaddr(uint32_t addr)
{
    lock_guard<mutex> lock(this->_allocatedIPsMapMutex);
    if (this->_allocatedIPs->count(addr))
    {
        pair<time_t, vector<IPv4Prefix*>> val = this->_allocatedIPs->at(addr);
        for (auto &prefix : val.second)
        {
            prefix->removeAddress(addr);
            // Update consol
            if(this->_consolFlag)
            {
                this->cl.updateConsol(prefix->getPrefixName());
            }
        }

        this->_allocatedIPs->erase(addr);
    }
}


void DHCPMonitor::_leaseTimeCheck()
{
    time_t currTime = time(nullptr); // get current time
    // Iterate trough all IP address in map and erase those with expired lease time
    // If lease time is 0 it means that lease time was not provided for particular ip address (it is considered an infinite lease time)
    for (auto it = this->_allocatedIPs->cbegin(); it != this->_allocatedIPs->cend();)
    {
        if ((currTime >= it->second.first) && (it->second.first != 0))
        {
            // IT IS NOT POSSIBLE TO USE DHCPMonitor::_removeIPaddr(uint32_t addr) bcs. it cause deadlock in _leaseTimeChecker()
            // Remove address from all prefixes
            for (auto &prefix : it->second.second)
            {
                prefix->removeAddress(it->first);
                // Update consol
                if(this->_consolFlag)
                {
                    this->cl.updateConsol(prefix->getPrefixName());
                }
            }
            this->_allocatedIPs->erase(it++);
        }
        else
        {
            ++it;
        }
    }
}


void DHCPMonitor::_leaseTimeChecker(DHCPMonitor *monitor)
{
    while (!monitor->_stopFlag.load())
    {
        unique_lock<mutex> lock(monitor->_allocatedIPsMapMutex); // lock mutex
        monitor->_leaseTimeCheck();
        monitor->_stopC.wait_for(lock, chrono::seconds(monitor->_leaseTimeCheckFrequency)); // open mutex and wait
    }
}

/* END OF FILE */