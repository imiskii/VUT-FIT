/**
 * @file nscan.cpp
 * @author Michal Ľaš
 * @brief main function for ipk-l4-scan project
 * @date 2023-03-05
 * 
 */


#include "nscan.hpp"


using namespace std;


int main(int argc, char *argv[])
{

    ArgParser ArgumentParser(argc, argv);
    arguments args = ArgumentParser.parse();

    // Seed the random number generator
    random_device rd;
    mt19937 mt(rd());

    // Define the range for the random port number
    int min = 49152;
    int max = 65535;

    // Random port number generator
    uniform_int_distribution<int> gen_port_num(min, max);

    // Construct headers
    struct iphdr IPv4Header;
    struct ip6_hdr IPv6Header;

    struct tcphdr TCPHeader;
    createTCPHeader(&TCPHeader, gen_port_num(mt), 0, rand());

    struct udphdr UDPHeader;
    createUDPHeader(&UDPHeader, gen_port_num(mt), 0);

    // get local ip address
    string localIPv4 = getLocalIP(IPv4);
    string localIPv6 = getLocalIP(IPv6);
    // choose interface and ip addres that will be used
    string localIPaddr = ""; // will be set later
    string useInterface = args.interface;

    // Check if at least one local addres was found
    if (localIPv4 == "" && localIPv6 == "")
    {
        exit_error(ErrorCodes::FAILURE, "ERR: Unable to find local IP address !");
    }

    // Set up interupt signal handler
    signal(SIGINT, interuptSignalHandler);

    // loop through ip addresses 
    for (int j = 0; j < args.ip.size(); j++)
    {
        /*** Scap TCP ports ***/

        // choose local IP address
        if (args.ip.at(j) == "127.0.0.1") // local IPv4 host
        {
            localIPaddr = "127.0.0.1";
        }
        else if (args.ip.at(j) == "::1") // local IPv6 host
        {
            localIPaddr = "::1";
        }
        else // other
        {
            localIPaddr = (args.ip_type.at(j) == IPv4) ? localIPv4 : localIPv6;
        }

        // check type of ip address
        if (localIPaddr == "")
        {
            print_warning("Cannot scan address '", args.ip.at(j), "' because local IP address of version ", args.ip_type.at(j)," is not available.");
            continue;
        }

        // if target is localhost set interface as local loopback else keep it on given interface that should be used
        if (args.ip.at(j) == "127.0.0.1" || args.ip.at(j) == "::1" || args.ip.at(j) == localIPv4 || args.ip.at(j) == localIPv6)
        {
            useInterface = args.loopback;
        }
        else
        {
            useInterface = args.interface;
        }

        // Set ip header
        if (args.ip_type.at(j) == 4)
        {
            createIPv4Header(TCP_P, &IPv4Header, args.ip.at(j), localIPaddr);
        }
        else
        {
            createIPv6Header(TCP_P, &IPv6Header, args.ip.at(j), localIPaddr);
        }

        int TCPsock = getTCPSocket(GET_SOCKET, args.ip_type.at(j));
        TCPScanner TCP_scan(useInterface, args.ip.at(j), localIPaddr, args.ip_type.at(j), args.timeout, TCPsock, &IPv4Header, &IPv6Header, &TCPHeader);

        cout << "Interesting ports on " << args.hostname << " (" << args.ip.at(j) << "):" << endl;
        cout << "PORT STATE" << endl;
        // loop through ports
        for (int i = 0; i < args.pt.size(); i++)
        {
            // Scan TCP ports
            string TCP_result = TCP_scan.TCPScan(args.pt.at(i));
            if(TCP_result != "")
            {
                cout << args.pt.at(i) << "/tcp " << TCP_result << endl;
            }
            // Create new tcp header
            createTCPHeader(&TCPHeader, gen_port_num(mt), 0, rand());
        }

        getTCPSocket(DEL_SOCKET);

        /*** Scap UDP ports ***/

        // Set ip header
        if (args.ip_type.at(j) == 4)
        {
            createIPv4Header(UDP_P, &IPv4Header, args.ip.at(j), localIPaddr);
        }
        else
        {
            createIPv6Header(UDP_P, &IPv6Header, args.ip.at(j), localIPaddr);
        }

        int UDPsock = getUDPSocket(GET_SOCKET, args.ip_type.at(j));
        UDPScanner UDP_scan(useInterface, args.ip.at(j), localIPaddr, args.ip_type.at(j), args.timeout, UDPsock, &IPv4Header, &IPv6Header, &UDPHeader);

        // loop through ports
        for (int i = 0; i < args.pu.size(); i++)
        {
            // Scan UDP ports
            string UDP_result = UDP_scan.UDPScan(args.pu.at(i));
            if (UDP_result != "")
            {
                cout << args.pu.at(i) << "/udp " << UDP_result << endl;
            }
            createUDPHeader(&UDPHeader, gen_port_num(mt), 0);
        }

        getUDPSocket(DEL_SOCKET);
    }

    return 0;
}



string getLocalIP(int IPv)
{
    struct ifaddrs *addr_list, *addr;
    char host[NI_MAXHOST];

    if (getifaddrs(&addr_list) == -1) {
        perror("ERR: getifaddrs");
        exit(EXIT_FAILURE);
        return "";
    }

    for (addr = addr_list; addr != nullptr; addr = addr->ifa_next) {
        if (addr->ifa_addr == nullptr) {
            continue;
        }

        int family = addr->ifa_addr->sa_family;
        if ((IPv == 4) && (family = AF_INET))
        {
            int s = getnameinfo(addr->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            if (s == 0 && std::string(host) != "127.0.0.1")
            {
                freeifaddrs(addr_list);
                return string(host);
            }
        }
        else if ((IPv == 6) && (family == AF_INET6))
        {
            int s = getnameinfo(addr->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
            if (s == 0 && std::string(host) != "::1")
            {
                freeifaddrs(addr_list);
                string ip = string(host);
                size_t index = ip.find("%");
                if (index != string::npos)
                {
                    ip = ip.erase(index);
                }
                
                return ip;
            }
        }
    }
    
    freeifaddrs(addr_list);
    return "";
}


int getTCPSocket(int mode, int ip_type)
{
    static int TCPsock;
    
    if(mode) // get socket
    {
        if(TCPsock <= 0) // Create TCP socekt
        {
            TCPsock = socket(((ip_type == 6) ? AF_INET6 : AF_INET), SOCK_RAW, IPPROTO_TCP);
            if (TCPsock <= 0) 
            {
                perror("ERR: socket");
                getUDPSocket(DEL_SOCKET);
                exit(EXIT_FAILURE);
            }

            // Set information that headers are included in sended data
            int valTrue = 1;
            const int *valPtr = &valTrue;
            if (setsockopt(TCPsock, ((ip_type == 4) ? IPPROTO_IP : IPPROTO_IPV6), ((ip_type == 4) ? IP_HDRINCL : IPV6_HDRINCL), valPtr, sizeof(valTrue)) < 0)
            {
                perror("ERR: setsockopt");
                close(TCPsock);
                getUDPSocket(DEL_SOCKET);
                exit(EXIT_FAILURE);
            }
        }
        return TCPsock;
    }
    else if(!mode && TCPsock) // delete TCP socket
    {
        close(TCPsock);
        TCPsock = 0;
    }
    return 0;
}


int getUDPSocket(int mode, int ip_type)
{
    static int UDPsock;

    if(mode) // get socket
    {
        if(UDPsock <= 0) // Create UDP socekt
        {
            UDPsock = socket(((ip_type == 6) ? AF_INET6 : AF_INET), SOCK_RAW, IPPROTO_UDP);
            if (UDPsock <= 0) 
            {
                perror("ERR: socket");
                getTCPSocket(DEL_SOCKET, ip_type);
                exit(EXIT_FAILURE);
            }
            // Set information that headers are included in sended data
            int valTrue = 1;
            const int *valPtr = &valTrue;
            if (setsockopt(UDPsock, ((ip_type == 4) ? IPPROTO_IP : IPPROTO_IPV6), ((ip_type == 4) ? IP_HDRINCL : IPV6_HDRINCL), valPtr, sizeof(valTrue)) < 0)
            {
                perror("ERR: setsockopt");
                close(UDPsock);
                getTCPSocket(DEL_SOCKET, ip_type);
                exit(EXIT_FAILURE);
            }
        }
        return UDPsock;
    }   
    else if(!mode && UDPsock) // delete UDP socket
    {
        close(UDPsock);
        UDPsock = 0;
    }
    return 0;
}


void interuptSignalHandler(int signum)
{
    getTCPSocket(DEL_SOCKET);
    getUDPSocket(DEL_SOCKET);
    exit(signum);
}


/* END OF FILE */