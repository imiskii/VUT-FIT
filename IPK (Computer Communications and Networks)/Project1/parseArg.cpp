/**
 * @file parseArg.cpp
 * @author Michal Ľaš
 * @brief argument parsing and processing for ipkcp
 * @date 2023-03-21
 * 
 */


#include "parseArg.hpp"


void print_help()
{
    std::cout << "ipkcp is a client for the IPK Calculator Protocol. This application can connect to remote calculator using IPKCP.\n" 
              << "Required parameters:\n"
              << "\t -h [host]\n" 
              << "\t -p [port]\n"
              << "\t -m [mode] - it can be either 'tcp' or 'udp'\n" 
              << "\n To print help use single parameter '--help'\n" << std::endl;
    exit(0);
}


bool checkIP(std::string ip)
{
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) != 0;
}


void checkPort(int port)
{
    if (65536 < port || 0 >= port)
    {
        exit_error(ErrorCodes::PROG_ARG_ERROR, "Given port '", std::to_string(port), "' is out of possible port range !");
    }
}


std::string getIPfromHostname(std::string host)
{
    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL)
    {
        exit_error(ErrorCodes::PROG_ARG_ERROR, "'", host, "' is not a either valid hostname or IPv4 address!");
    }

    for (int i = 0; server->h_addr_list[i] != nullptr; i++) {
        if (server->h_addrtype == AF_INET) { // check for IPv4 address only
            struct in_addr addr; // something more simple that struct sockaddr_in
            memcpy(&addr, server->h_addr_list[i], sizeof(struct in_addr));
            return std::string(inet_ntoa(addr));
        }
    }
    
    exit_error(ErrorCodes::PROG_ARG_ERROR, "No IPv4 address was found for hostname '", host, "'.");
    return "";
}


ProgramArgs ArgParse(int argc, char *argv[])
{
    // Check print help argument
    if (argc == 2 && !strcmp(argv[1], "--help"))
    {
        print_help();
    }

    // Check number of arguments
    if (argc != 7)
    {
        exit_error(ErrorCodes::PROG_ARG_ERROR, "Invalid number of arguments ! Required format: -h [host] -p [port] -m [mode] or single --help argument for printing program information.");
    }

    // process arguments
    ProgramArgs args = {.ip_addr = "", .port = 0, .mode = ""};

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h"))
        {
            if (args.ip_addr != "")
                exit_error(ErrorCodes::PROG_ARG_ERROR, "Redeclaration of program argument: ", argv[i]);
            i++;
            args.ip_addr = argv[i];
            // it is not IPv4 address => check if it is hostname
            if (!checkIP(args.ip_addr))
                args.ip_addr = getIPfromHostname(args.ip_addr);
        }
        else if (!strcmp(argv[i], "-p"))
        {
            if (args.port != 0)
                exit_error(ErrorCodes::PROG_ARG_ERROR, "Redeclaration of program argument: ", argv[i]);
            i++;
            args.port = std::atoi(argv[i]);
            checkPort(args.port);
        }
        else if (!strcmp(argv[i], "-m"))
        {
            if (args.mode != "")
                exit_error(ErrorCodes::PROG_ARG_ERROR, "Redeclaration of program argument: ", argv[i]);
            i++;
            args.mode = argv[i];
        }
        else
        {
            exit_error(ErrorCodes::PROG_ARG_ERROR, "Unknow program argument '", argv[i], "' !");
        }
    }
    return args;
}