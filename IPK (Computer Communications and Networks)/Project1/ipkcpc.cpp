/**
 * @file ipkcpc.cpp
 * @author Michal Ľaš
 * @brief IPK Project 1: IPK Calculator Protocol
 * @date 2023-02-22
 * 
 */


#include "ipkcpc.hpp"


/****************** MAIN ********************/

int main(int argc, char *argv[])
{

    // Checking program arguments
    ProgramArgs args = ArgParse(argc, argv);

    if (args.mode == "udp")
    {
        use_UDP(args.ip_addr, args.port);
    }
    else if (args.mode == "tcp")
    {
        use_TCP(args.ip_addr, args.port);
    }
    else
    {
        exit_error(ErrorCodes::PROG_ARG_ERROR, "Unknow mode '", args.mode, "'. This program support only 'udp' or 'tcp'");
    }


    return 0;
}


void use_UDP(std::string ip, int port)
{
    // Create new Socket
    int client_socket = getUDPSocket(GET_SOCK);

    // Set up interupt signal handler
    signal(SIGINT, UDPSignalHandler);

    // Socket address
    struct sockaddr_in server_address;
    
    // Set address family, port and IP
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip.c_str());

    // Set up timeout
    struct timeval tv;
    tv.tv_sec = UDP_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("ERR: setsockopt");
        exit(EXIT_FAILURE);
    }

    // User input
    std::string input;
    // Server resose
    std::string response;

    // Iterate through input data
    while (getline(std::cin, input))
    {
        response = IPKCP_UDP(server_address, client_socket, input);

        if (response != "")
        {
            std::cout << "OK:" << response << std::endl;
        }
    }
    
    // close socket
    closeSocketUDP(client_socket);
    
}


void use_TCP(std::string ip, int port)
{
    // Create new Socket
    int client_socket = getTCPSocket(GET_SOCK);

    // Set up interupt signal handler
    signal(SIGINT, TCPSignalHandler);

    // Socket address
    struct sockaddr_in server_address;

    // Set address family, port and IP
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

    // Connect to server
    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) != 0)
    {
        perror("ERR: connect");
        exit(EXIT_FAILURE);
    }

    // User input
    std::string input;
    // Server resose
    std::string response;

    // Iterate through input data
    while (response != "BYE\n")
    {
        // Read input
        if (!getline(std::cin, input))
        {
            // if EOF
            closeTCPConnection();
            break;
        }

        response = IPKCP_TCP(client_socket, input);

        if (response != "")
        {
            std::cout << response;
        }
    }

    // close socket
    closeSocketTCP(client_socket);
}


/* END OF FILE */