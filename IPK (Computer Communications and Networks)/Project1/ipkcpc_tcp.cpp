/**
 * @file ipkcpc_tcp.cpp
 * @author Michal Ľaš xlasmi00
 * @brief TCP variant for comunication with IPKCP
 * @date 2023-03-21
 *
 */


#include "ipkcpc_tcp.hpp"


void closeSocketTCP(int &client_socket)
{
    if (client_socket > 0)
    {
        // close the socket for further receptions and transmissions
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);

        client_socket = 0;
        getTCPSocket(DEL_SOCK);
    }
}

void closeTCPConnection()
{
    int trys = BYE_TRYS; // maximum trys to say BYE to server
    int sock = getTCPSocket(GET_SOCK);
    std::string msgReceived;

    // Buffer for message recived from server
    char buffer[BUFFER_SIZE];

    while(msgReceived != "BYE\n" || trys <= 0) // while it is not BYE message
    {
        trys--;
        // Send BYE message to server
        int bytes_tx = send(sock, "BYE\n", 4, 0);
        if (bytes_tx < 0)
        {
            perror("ERR: send");
            continue;
        }

        // Recive data
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytesReceived < 0)
        {
            perror("ERR: recv");
            continue;
        }

        msgReceived.clear();
        msgReceived = std::string(buffer, bytesReceived);
    }

    closeSocketTCP(sock);

    if (trys <= 0 && msgReceived != "BYE\n")
    {
        exit_error(ErrorCodes::SEND_ERROR, "Unable to say last 'BYE' to server !");
    }

    // print BYE
    std::cout << msgReceived;
}


void TCPSignalHandler(int signum)
{
    // Say BYE to server
    closeTCPConnection();
    exit(signum);
}


int getTCPSocket(int flag)
{
    static int sock = 0;

    if (flag)
    {
        if (sock <= 0)
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock <= 0)
            {
                perror("ERR: socket");
                exit(EXIT_FAILURE);
            }
        }   
    }
    else
    {
        sock = 0;
    }

    return sock;
}


std::string IPKCP_TCP(int client_socket, std::string msg)
{
    // Buffer for message recived from server
    char buffer[BUFFER_SIZE];
    // Message recived from server
    std::string msgReceived;
        
    // Add eol to input (LF)
    msg += '\n';

    // Send message to server
    int bytes_tx = send(client_socket, msg.c_str(), msg.size(), 0);
    if (bytes_tx < 0)
    {
        perror("ERR: send");
        return "";
    }

    // Recive data
    memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived < 0)
    {
        perror("ERR: recv");
        return "";
    }

    // Print message from server
    msgReceived.clear();
    msgReceived = std::string(buffer, bytesReceived);
    return msgReceived;
}


/* END OF FILE */