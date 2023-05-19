/**
 * @file argparse.cpp
 * @author Michal Ľaš
 * @brief Functions and classes for argument parsing
 * @date 2023-03-05
 * 
 */


#include "argparse.hpp"


using namespace std;


ArgParser::ArgParser(int argc, char **argv)
{
    this->__short_opt = "hi::u:t:w:";
    this->__long_opt = {
        {"help", no_argument, nullptr, 'h'},
        {"interface", optional_argument, nullptr, 'i'},
        {"pu", required_argument, nullptr, 'u'},
        {"pt", required_argument, nullptr, 't'},
        {"wait", required_argument, nullptr, 'w'},
        {nullptr, 0, nullptr, 0}
    };
    this->flags = {false, false, false, false, false, false};
    this->__argCount = argc;
    this->__progArgs = argv;
}

ArgParser::~ArgParser()
{
}


arguments ArgParser::parse()
{
    // option
    int op = 0; 

    if (this->__argCount == 1)
    {
        this->__printInterfaces();
    }

    // process program options (arguments)
    while((op = getopt_long(this->__argCount, this->__progArgs, this->__short_opt.c_str(), this->__long_opt.data(), nullptr)) != -1)
    {
        switch (op)
        {
        // help option
        case 'h':
            if (this->__argCount == 2) // single argument '-h' or '--help' is used
            {
                this->__printHelp();
            }
            else
            {
                exit_error(ErrorCodes::PROG_ARG_ERR, "To print help use single argument '-h' or '--help' !");
            }
            break;
        // interface option
        case 'i':
            if (this->__argCount == 2) // single argument '-i' or '--interface' is used
            {
                // print list of active interfaces
                this->__printInterfaces();
            }
            else
            {
                // It has a argument in form -iARGUMENT (without space)
                if (optarg != nullptr)
                {
                    this->flags.i_flag = true;
                    this->args.interface = optarg;
                }
                // This is kind of trick bcs. 'i' could has an optional argument and getopt() can not process his argument
                // when space is between -i and his argument in general it can process argument in this forme: -iARGUMENT without space
                else if (optarg == nullptr && optind < this->__argCount && this->__progArgs[optind][0] != '-')
                {
                    this->flags.i_flag = true;
                    this->args.interface = this->__progArgs[optind++];
                }
                // Missing option argument
                else
                {
                    exit_error(ErrorCodes::PROG_ARG_ERR, "Undefined interface ! To print active interfaces use single argument '-i' or '--interface'.");
                }

                // Validate given interface - check if given interface is active on current device
                if (!this->__isActiveInterface(this->args.interface))
                        exit_error(ErrorCodes::PROG_ARG_ERR, "'", this->args.interface, "' is not an active network interface on your device ! To print active interfaces use single argument '-i' or '--interface'.");
            }
            break;
        // UDP port range option
        case 'u':
            this->flags.u_flag = true;
            this->args.pu = this->__parsePortRanges(optarg);
            break;
        // TCP port range option
        case 't':
            this->flags.t_flag = true;
            this->args.pt = this->__parsePortRanges(optarg);
            break;
        // timeout option
        case 'w':
            this->flags.w_flag = true;
            this->args.timeout = atoi(optarg);
            if (this->args.timeout == 0)
            {
                exit_error(ErrorCodes::PROG_ARG_ERR, "Invalid timeout argument: ", string(optarg), " !");
            }
            break;
        default:
            // Unknow argument
            exit_error(ErrorCodes::PROG_ARG_ERR, "Unknow program argument !");
            break;
        }
    }
    
    // process hostname or ip address
    if (optind < this->__argCount)
    {
        this->flags.ip_flag = true;
        this->args.hostname = "IP address";

        if (this->__isValidIPv4(this->__progArgs[optind]))    // if it is IPv4
        {
            this->args.ip.push_back(__progArgs[optind]);
            this->args.ip_type.push_back(IPv4);
        }
        else if (this->__isValidIPv6(this->__progArgs[optind]))   // if it is IPv6
        {
            string ip = string(__progArgs[optind]);
            size_t index = ip.find("%");
                if (index != string::npos)
                {
                    ip = ip.erase(index);
                }
            this->args.ip.push_back(ip);
            this->args.ip_type.push_back(IPv6);
        }
        else    // if it is hostname
        {
            this->args.hostname = this->__progArgs[optind];
            this->__getIPfromHost(this->__progArgs[optind]);
        }
        
    }
    else // missing hostname or ip address in program arguments
    {
        exit_error(ErrorCodes::PROG_ARG_ERR, "Missing IP address or hostname that will be scaned !");
    }
    
    if (!this->flags.i_flag || !this->flags.ip_flag || !(this->flags.u_flag || this->flags.t_flag))
        exit_error(ErrorCodes::PROG_ARG_ERR, "Missing required program argument ! Required program arguments are interface, hostname/ip address, ports to be scanned.");

    // if timeout was not given use default timeout (5000)
    if (!this->flags.w_flag)
        this->args.timeout = DEFAULT_TIMEOUT;
    
    return this->args;
}


void ArgParser::__printHelp()
{
    cout << "\nThe ipk-l4-scan is TCP and UDP network L4 scanner."
         << "The program scan the specified hostname or IP address (either IPv4 or IPv6) and ports. It output to stdout port statuses (open, filtered or closed).\n"
         << "\nTo print information about porgram usage use singel single argument -h or --help\n"
         << "\nRequired arguments:\n"
         << "\n-i interface | --interface interface\n\t\t interface to scan through\n"
         << "\n-u port-ranges | --pu port-ranges\n\t\t port-ranges for scanning UDP ports\n"
         << "\n-t port-ranges | --pt port-ranges\n\t\t port-ranges for scanning TCP ports\n"
         << "\nip-address | domain-name\n\t\t target of scanning\n"
         << "\nOptional arguments\n"
         << "\n-w timeout | --wait timeout\n\t\t timeout in milliseconds to wait for a response for a single port scan (by default is set to 5000)\n"
         << "\nExecution:\n"
         << "./ipk-l4-scan [-i interface | --interface interface] [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges] {-w timeout} [domain-name | ip-address]\n"
         << endl;
    exit(EXIT_SUCCESS);
}


vector<tuple<string, string>> ArgParser::__getInterfaces()
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *interfaces;

    // vector of possible interface and their descriptions
    vector<tuple<string, string>> interface_vector;

    // Find all available interfaces
    if (pcap_findalldevs(&interfaces, errbuf) == -1) 
    {
        exit_error(ErrorCodes::PROG_ARG_ERR, "Error while finding network interfaces: ", string(errbuf));
    }
    

    for (pcap_if_t *iface = interfaces; iface != nullptr; iface = iface->next) 
    {
        string idescription = "";
        if (iface->description)
        {
            idescription = iface->description;
        }
        interface_vector.push_back(make_tuple(string(iface->name), string(idescription)));

        // save local loopback 
        if (iface->flags & PCAP_IF_LOOPBACK)
        {
            this->args.loopback = iface->name;
        }
    }

    // Free the list of interfaces
    pcap_freealldevs(interfaces);

    return interface_vector;
}


bool ArgParser::__isActiveInterface(string interface)
{
    for (auto [name, description] : this->__getInterfaces())
    {
        if (name == interface)
        {
            return true;
        }
    }
    return false;
}


void ArgParser::__printInterfaces()
{
    cout << "Finded active network interfaces: " << endl;
    for (auto [name, description] : this->__getInterfaces())
    {
        cout << "Name: " << name << endl;
        cout << "Description: " << description << endl;
        cout << endl;
        
    }
    exit(EXIT_SUCCESS);
}


vector<int> ArgParser::__parsePortRanges(char *range)
{
    vector<int> rangeValues;
    string s = string(range);


    size_t pos = s.find("-");
    // There is a range of ports
    if (pos != string::npos)
    {
        int from = atoi(s.substr(0, pos).c_str());
        int to = atoi(s.substr(pos+1).c_str());
        if (from != 0 && to != 0)
        {
            for (int i = from; from <= to; from++)
            {
                rangeValues.push_back(from);
            }
        }
        else
        {
            exit_error(ErrorCodes::PROG_ARG_ERR, "Invalid port range: ", string(range), " !");
        }
    }
    // There are ports separated by comma
    else if (s.find(",") != string::npos)    
    {
        char *ptr;
        ptr = strtok(range, ",");

        while (ptr != nullptr)
        {
            int port = atoi(ptr);
            if (port != 0)
            {
                rangeValues.push_back(port);
            }
            else
            {
                exit_error(ErrorCodes::PROG_ARG_ERR, "Invalid port: ", string(ptr), " !");
            }
            ptr = strtok(nullptr, ",");
        }
    }
    // There is a single port
    else
    {
        int port = atoi(range);
        if (port != 0)
        {
            rangeValues.push_back(atoi(range));
        }
        else
        {
            exit_error(ErrorCodes::PROG_ARG_ERR, "Invalid port: ", string(range), " !");
        }
    }

    return rangeValues;
}


bool ArgParser::__isValidIPv4(char *address)
{
    regex ipv4("\\s*^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\s*$");
    return regex_match(address, ipv4);
}


bool ArgParser::__isValidIPv6(char *address)
{
    // regex from IPK lectures (presentation about IPv6)
    regex ipv6("\\s*((([0-9A-Fa-f]{1,4}:){7}(([0-9A-Fa-f]{1,4})|:))|(([0-9A-Fa-f]{1,4}:){6}(:|((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})|(:[0-9A-Fa-f]{1,4})))|(([0-9A-Fa-f]{1,4}:){5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(([0-9A-Fa-f]{1,4}:)(:[0-9A-Fa-f]{1,4}){0,4}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(:(:[0-9A-Fa-f]{1,4}){0,5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|(((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})))(%.+)?\\s*$");
    return regex_match(address, ipv6);
}


void ArgParser::__getIPfromHost(char *host)
{
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    int status = getaddrinfo(host, nullptr, &hints, &result);
    if (status != 0) 
    {
        exit_error(ErrorCodes::PROG_ARG_ERR, "Unknow or invalid hostname: ", string(host), " !");
    }

    // Find the first IPv4 or IPv6 address in the list
    for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) 
    {
        if (rp->ai_family == AF_INET) // if IPv4
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            char ip_address[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);

            this->args.ip.push_back(string(ip_address));
            this->args.ip_type.push_back(IPv4);
        }
        if (rp->ai_family == AF_INET6) // if IPv6
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
            char ip_address[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ip_address, INET6_ADDRSTRLEN);
            
            this->args.ip.push_back(string(ip_address));
            this->args.ip_type.push_back(IPv6);
        }
    }
    // Nothing found
    if (args.ip.empty())
    {
        exit_error(ErrorCodes::PROG_ARG_ERR, "Unable to find IP address for: ", string(host), ".");
    }
}


/* END OF FILE */