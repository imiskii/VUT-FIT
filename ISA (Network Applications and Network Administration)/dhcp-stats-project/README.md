# DHCP-stats

### Author: Michal Ľaš (xlasmi00)
### Date: Creation 19.10.2023


## Brief

This program allows retrieving information about network prefix utilization in terms of allocated IP addresses. 
Upon execution, the program begins monitoring DHCP traffic on the selected interface or processes a specified pcap file.
It generates statistics on the utilization of the network prefix provided in the command line.
When the prefix is filled to more than 50\%, the tool notifies the administrator and creates a log using a syslog server.

## Functionality

+ This application specifically monitors DHCP acknowledgment and DHCP release packets. When processing a DHCPACK packet, 
a new IP address is allocated within the corresponding prefixes. Upon receiving a DHCPRELEASE packet, the sender's (source) IP address is removed from the list of allocated addresses. If DHCP acknowledgment is received for already allocate IP address, then only lease time of IP address is updated.

+ The application requires either a pcap file that it analyzes or an Internet interface on which it will monitor DHCP packets. 
If both the pcap file and the Internet interface are specified in the program arguments, then after processing the pcap file, the application continues processing packets on the specified network interface.

+ The lease time is checked for packets in the pcap file only when the network interface is specified in the program arguments.
Lease time for network communication is always checked.
This approach ensures that the monitored communication statistics remain current. Additionally, it allows for the analysis of older DHCP communication statistics.
If the lease time of the IP address has expired, the IP address is removed from the list of allocated IP addresses.


## Limitations

+ If the client responds to the DHCPACK message with a DHCPDECLINE message, the IP address will still remain allocated. 
Consequently, the prefixes associated with this address will continue to include it in the statistics.

+ If a DHCPNAK message is sent in response to a DHCPREQUEST message intended to renew the lease time of the IP address, the IP address will persist in allocation. 
Consequently, the prefixes to which this address belongs will continue to include it in the statistics.

+ If the DHCP packet contains the _option overload_, incorrect processing of the packet may occur, 
especially if the _message type_ and _lease time_ options fall outside the standard options field in the DHCP packet.

+ Logs and warning messages are created only if network interface is specified. This is because it make no sense to create logs or send warning messages
during just pcap file analysis. However, if network interface is specified in program arguments then warning notifications from pcap file analysis are 
printed to _stdout_ and notifications from network analysis are printed to program **console**.

+ This program can proccess packets that are structured only like this:  
_Ethernet header_ + _IPv4 header_ + _UDP header_ + _DHCP header_.


## Example of usage

./dhcp-stats [-i _interface_ | --interface _interface_] [-r _pcap-file_ | --read _pcap-file_] [-t _seconds_ | --lctime _seconds_] ip-prefix [ ip-prefix [ ... ]]

**Required program arguments:**

+ -i _interface_ | --interface _interface_ - Specifies the network interface where the dhcp-stats tool will monitor DHCP packets. If a single \texttt{-i | --interface} argument is provided, the application will list available interfaces
+ -r _pcap-file_ | --read _pcap-file_ - Pcap file from where dhcp-stats tool can read previously monitored DHCP traffic
+ ip-prefix [ ip-prefix [ ... ]] - IPv4 prefixes that will be monitored

**Optional program arguments:**

+ -t _seconds_ | --lctime _seconds_ - Frequency of lease time check for monitored ip addresses in seconds (default is 60 seconds)

## List of content

+ src
    - argpares.cpp
    - argparse.hpp
    - consol-log.cpp
    - consol-log.hpp
    - dhcp-monitor.cpp
    - dhcp-monitor.hpp
    - dhcp-stats.cpp
    - dhcp-stats.hpp
    - errors.cpp
    - errors.hpp
    - ip-prefix.cpp
    - ip-prefix.hpp
    - Makefile
+ doc
    - manual.pdf
    - images
+ dhcp-stats.1
+ REDME.md

