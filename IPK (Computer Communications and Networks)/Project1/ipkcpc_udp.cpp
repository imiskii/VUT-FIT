/**
 * @file ipkcpc_udp.cpp
 * @author Michal Ľaš xlasmi00
 * @brief UDP variant for comunication with IPKCP
 * @date 2023-03-21
 * 
 */


#include "ipkcpc_udp.hpp"


void IPKCPmsgEncode(std::string data, unsigned char *msg)
{
    // Add opcode and size of payload data
    msg[0] = 0; // opcode
    msg[1] = data.size(); // size of data
    // Add the data to message
    memcpy(msg + 2, data.c_str(), data.size());
}


bool IPKCPmsgDecode(char *data)
{
    if (data[0] != 1)
    {
        // do not get response opcode
        print_warning("Got message with invalid opcode '", std::to_string(data[0]), "'. Expected opcode '1'.");
        return false;
    }
    if (data[1])
    {
        // there is error in message
        std::cout << "ERR:" << (data+3) << std::endl;
        return false;
    }
    // size of recived data
    std::size_t msgSize = data[2];
    std::string msg = string(data + 3);
    if (msgSize != msg.size())
    {
        print_warning("Received message has length: '", msg.size(), "', but packed declared size: '", msgSize, "' !");
    }

    return true;
}


void closeSocketUDP(int &client_socket)
{
    if (client_socket > 0)
    {
        // close the socket
        close(client_socket);

        client_socket = 0;
        getUDPSocket(DEL_SOCK);
    }
}


void UDPSignalHandler(int signum)
{
    int sock = getUDPSocket(GET_SOCK);
    closeSocketUDP(sock);
    exit(signum);
}


int getUDPSocket(int flag)
{
    static int sock = 0;

    if (flag)
    {
        if (sock <= 0)
        {
            sock = socket(AF_INET, SOCK_DGRAM, 0);
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


std::string IPKCP_UDP(sockaddr_in server_address, int client_socket, std::string msg)
{
    // Address size
    socklen_t address_size = sizeof(server_address);

    // Buffer for message recived from server
    char buffer[BUFFER_SIZE];

    // Array of bytes with payload data
    unsigned char payloadData[msg.size() + 2];
    memset(&payloadData, 0, msg.size() + 2);
    // Opcode + size of message + message
    int payloadLength = msg.size() + 2;

    // Encode message for server with IPKCP protocol
    IPKCPmsgEncode(msg, payloadData);

    // Send message to server
    int bytes_tx = sendto(client_socket, payloadData, payloadLength, 0, (struct sockaddr*) &server_address, address_size);
    if (bytes_tx < -1)
    {
        perror("ERR: sendto");
        return "";
    }

    // Recive data
    memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &server_address, &address_size);
    if (bytesReceived < 0)
    {
        perror("ERR: recvfrom");
        return "";
    }

    // Decode message
    if (!IPKCPmsgDecode(buffer))
    {
        return "";
    }

    return string((buffer + 3));
}


/* END OF FILE */