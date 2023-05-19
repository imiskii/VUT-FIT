
# Changelog

## 15.04.2023

### ipk-l4-scan version 1.0

**Functionality**

+ TCP scanning (IPv6 and IPv4)
+ UDP scanning (IPv6 and IPv4)
+ interrupt (C-c) signal handler added
+ If localhost (IPv6 or IPv4) address is given as target address then local loopback is implicitly set as interface

**Limitations**

+ Scanning IPv4 addresses is possible only if the device on which the program is running has a valid IPv4 address, this also applies to IPv6
+ The program can successfully process only packets with data link header `Ethernet` or `Linux SSL`

### Fix IPv6 TCP and UDP scanning

**Functionality**

+ TCP scanning (IPv6 and IPv4)
+ UDP scanning (IPv6 and IPv4)
+ interrupt (C-c) signal handler added
+ If localhost IPv6 or IPv4 address is given as target address then interface is switched automatically to local loopback interface

**New**

+ fix: IPv6 TCP and UDP port scanning (checksum and packet parsing)


## 13.04.2023

### Fix multiple port scanning

**Functionality**

+ TCP scanning
+ UDP scanning
+ interrupt (C-c) signal handler added
+ If localhost IPv6 or IPv4 address is given as target address then interface is switched automatically to `lo`

**New**

+ fix: bug that cause that only one port could be successfully scanned is fixed
+ bug: IPv6 port scanning gives no response on testing machine (TCP - all ports are filtered, UDP - all ports are open)


## 12.04.2023

### Socket creation update

**Functionality**

+ TCP scanning
+ UDP scanning
+ interrupt (C-c) signal handler added
+ If localhost IPv6 or IPv4 address is given as target address then interface is switched automatically to `lo`

**New**

+ bug: unable to successfully scan more than one port (other ports results are filtered)
+ feat: interrupt (C-c) signal handler added
+ feat: add possibility to scan all targets IP addresses
+ change: error handling in port scanning
+ change: functions for socket creation and deletion


## 09-04-2023

### UDP Scanning added

**Functionality**

+ TCP scanning
+ UDP scanning
+ If localhost IPv6 or IPv4 address is given as target address then interface is switched automatically to `lo`

**New**

+ Added UDP scanning


### TCP timeout added, little fixes and code restructuring

**Functionality**

+ TCP scanning
+ If localhost IPv6 or IPv4 address is given as target address then interface is switched automatically to `lo`

**New**

+ Timeout after no response fixed with `settimer()` function
+ TCP scanning added


## 07-04-2023

### TCP Scanning added

+ TCP scanning work for open ports
+ bug: Timeout is not working because `pacp_open_live()` function timeout work differently, so filtered ports can not be detected and program is stuck.  
Discussion here: [https://github.com/the-tcpdump-group/libpcap/issues/572](https://github.com/the-tcpdump-group/libpcap/issues/572)