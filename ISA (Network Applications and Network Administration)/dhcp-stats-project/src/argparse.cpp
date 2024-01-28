/**
 * @file argparse.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief classes, function and methods for dhcp-stats program argument parsing
 * @date 2023-10-19
 * 
 */


#include "argparse.hpp"

using namespace std;


ArgParser::ArgParser(int argc, char **argv) : __argCount(argc), __progArgs(argv)
{
    this->__short_opt = "hr:i::t:";
    this->__long_opt = {
        {"help", no_argument, nullptr, 'h'},
        {"interface", optional_argument, nullptr, 'i'},
        {"read", required_argument, nullptr, 'r'},
        {"lctime", required_argument, nullptr, 't'},
        {nullptr, 0, nullptr, 0}
    };
    this->flags = {false, false, false, false, false};
}

ArgParser::~ArgParser()
{
    for (auto &prefix : this->args.ip_prefixes)
    {
        delete prefix;
    }
}


arguments ArgParser::parse()
{
    // option
    int op = 0; 

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
                // This is kind of trick bcs. 'i' could has an optional argument and getopt() can not process this argument
                // when space is between -i and its argument in general it can process argument in this forme: -iARGUMENT without space
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
        // file option
        case 'r':
            if (this->isValidPcapFile(optarg))
            {
                this->flags.r_flag = true;
                this->args.pcap_file = optarg;
            }
            else
            {
                print_warning("Given pcap file (", optarg, ") is invalid, wrong file extension or missing file !");
            }
            break;
        // lease time check option
        case 't':
            this->flags.t_flag = true;
            this->args.leaseCheckSec = atol(optarg);
            if (this->args.leaseCheckSec == 0)
            {
                exit_error(ErrorCodes::PROG_ARG_ERR, "Invalid lease check frequency argument (--lctime/-t): ", string(optarg), " !");
            }
            break;
        default:
            // Unknow argument
            exit_error(ErrorCodes::PROG_ARG_ERR, "Unknow program argument !");
            break;
        }
    }
    
    // process ip prefixes
    if (optind < this->__argCount)
    {
        for (int i = optind; i < this->__argCount; i++)
        {
            if (this->isValidIPv4Prefix(this->__progArgs[i]))
            {
                this->flags.p_flag = true;
                IPv4Prefix *prefix = new IPv4Prefix(string(this->__progArgs[i]));
                this->args.ip_prefixes.push_back(prefix);
            }
            else
            {
                print_warning("IP prefix: '", this->__progArgs[i], "' is not valid IPv4 prefix !");
            }
        }
    }
    else // missing ip prefixes
    {
        exit_error(ErrorCodes::PROG_ARG_ERR, "Missing ip prefixes in program arguments ! To print help use single argument '-h' or '--help'");
    }
    
    // Check if all required arguments was given
    if (!(this->flags.i_flag || this->flags.r_flag) || !this->flags.p_flag)
        exit_error(ErrorCodes::PROG_ARG_ERR, "Missing required program argument ! Required program arguments are interface or pcap file and at least one ip prefix");
    
    // Use default lease time check frequency if it was not provided
    if (!this->flags.t_flag)
    {
        this->args.leaseCheckSec = DEFAULT_LEASE_TIME_CHECK;
    }
    
    return this->args;
}


void ArgParser::__printHelp()
{
    cout << "\nThe dhcp-stats is monitoring tool for dhcp traffic."
         << "The program works like console application, it prints given IPv4 prefixes and allocation of particular prefixes in network, once allocation of specific address exceeds 50% application print message and create syslog.\n"
         << "\nTo print information about porgram usage use singel single argument -h or --help\n"
         << "\nRequired arguments:\n"
         << "\n-i <interface> | --interface <interface>\n\t\t interface where dhcp-stats tool will be monitoring\n"
         << "\n-r <pcap-file> | --read <pcap-file>\n\t\t pcap file from where dhcp-stats tool can read previously monitored DHCP traffic\n"
         << "\n<ip prefix> | IPv4 prefixes that will be monitored\n"
         << "\nOptional arguments:\n"
         << "\n-t <seconds> | --lctime <seconds>\n\t\t frequency of lease time check for monitored ip addresses in seconds (lease time is checked only during online monitoring)\n"
         << "\nUsage:\n"
         << "\nThere has to be at least one IPv4 prefix that will be monitored (there can be more IPv4 prefixes)."
         << "There has to be at least one of the arguments -r | --read or -i | --interface, if there are both given application will analyze traffic in pcap file and continue to analyzing the traffic on given interface."
         << "\nIf single -i | --interface argument is given application will list available interfaces.\n"
         << "\nExecution:\n"
         << "\n./dhcp-stats [-i <interface-name> | --interface <interface-name>] [-r <pcap-file>| --read <pcap-file>] <ip-prefix> [ <ip-prefix> [ ... ] ]\n"
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


bool ArgParser::isValidIPv4Prefix(const char *address)
{
    regex ipv4WithPrefix("\\s*((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)/([0-9]|[1-2][0-9]|3[0-2])\\s*");
    return regex_match(address, ipv4WithPrefix);
}


bool ArgParser::isValidPcapFile(const char *pcap_file_path)
{
    filesystem::path path(pcap_file_path);
    if (path.has_extension() && (path.extension() == ".pcap" || path.extension() == ".pcapng")) // check if given file has .pcap extension
    {
        try // check if file exists
        {
            return filesystem::exists(path) && filesystem::is_regular_file(path);
        }
        catch (const filesystem::filesystem_error &ex)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/* END OF FILE */