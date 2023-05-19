/**
 * @file argparse.hpp
 * @author Michal Ľaš
 * @brief header file for argparse.cpp
 * @date 2023-03-05
 * 
 */


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <tuple>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pcap/pcap.h>
#include <regex>
#include "errors.hpp"


#ifndef ARGPARSE_H
#define ARGPARSE_H


using namespace std;


/******************** MACROS ********************/

#define IPv4 4
#define IPv6 6

#define DEFAULT_TIMEOUT 5000

/****************** DATATYPES *******************/


// parsed program arguments
struct arguments
{
    string interface;  // interface
    string loopback;   // local loopback
    vector<int> pu;    // UDP ports to scan
    vector<int> pt;    // TCP ports to scan
    int timeout;       // waiting time for response
    string hostname;   // given hostname
    vector<int> ip_type;       // IP_V4 (int 4) or IP_V6 (int 6)
    vector<string> ip;         // IP address either IPv4 or IPv6 
};


// argument flags (tells if program argument was given)
struct arg_flags  
{
    bool h_flag;    // help flag
    bool i_flag;    // interface flag
    bool u_flag;    // UDP ports flag
    bool t_flag;    // TCP ports flag
    bool w_flag;    // timeout flag
    bool ip_flag;   // IP address flag
};



/*********** FUNCTIONS AND CLASSES **************/



/**
 * @brief Class for program argument parsing
 * 
 * Attributes:
 * ---------------
 * vector<option> __long_opt: long program arguments options
 * string __short_opt: short program arguments options
 * int __argCount: number of arguments
 * char **__progArgs: program arguments
 * arguments args: structure with processed program arguments
 * arg_flags flags: structure with flags that telling if specific program argument was provided
 * 
 * Methods:
 * ---------------
 * void __printHelp(): print program help on terminal
 * vector<tuple<string, string>> __getInterfaces(): find usabel device interfaces
 * bool __isActiveInterface(string interface): check if given interface is active/available
 * void __printInterfaces(): print active/available on terminal
 * vector<int> __parsePortRanges(char *range): parse given port ranges
 * bool __isValidIPv4(char *address): validate IPv4 address
 * bool __isValidIPv6(char *address): validate IPv6 address
 * void __getIPfromHost(char *host): find ip addresses of given hostname
 * arguments parse(): parse program arguments and return structure with parsed program arguments
 * 
 */
class ArgParser
{
private:
    /* Private Attributes */
    vector<option> __long_opt;
    string __short_opt;
    int __argCount;
    char **__progArgs;
    /* Private Methods */

    /**
     * @brief print program help info
     * 
     */
    void __printHelp();

    /**
     * @brief find active/available interfaces on current device
     * 
     * @return vector<tuple<string, string>> tuples name of interface and description
     */
    vector<tuple<string, string>> __getInterfaces();
    
    /**
     * @brief Check if given interface is active/available on current device
     * 
     * @param interface name of interface
     * @return true if is available
     * @return false if is not available
     */
    bool __isActiveInterface(string interface);

    /**
     * @brief print active/available interfaces on current device
     * 
     */
    void __printInterfaces();

    /**
     * @brief parse range of ports that will be scaned to vector of int-s
     * 
     * @param range port range string from program arguments
     * @return vector<int> parsed ports
     */
    vector<int> __parsePortRanges(char *range);

    /**
     * @brief Check if given IPv4 address is valid
     * 
     * @param address ip address
     * @return true if it is valid IPv4 address
     * @return false if it is not valid IPv4 address
     */
    bool __isValidIPv4(char *address);

    /**
     * @brief Check if given IPv6 address is valid
     * 
     * @param address ip address
     * @return true if it is valid IPv6 address
     * @return false if it is not valid IPv6 address
     */
    bool __isValidIPv6(char *address);

    /**
     * @brief Find ip addresses of given hostname
     * 
     * @param host hostname
     */
    void __getIPfromHost(char *host);

public:
    /* Public Attributes */
    arguments args;
    arg_flags flags;
    /* Public Methods */

    /**
     * @brief Construct a new Arg Parser object
     * 
     * @param argc number of arguments
     * @param argv pointer to array with arguments
     */
    ArgParser(int argc, char **argv);
    ~ArgParser();

    /**
     * @brief Parse program arguments
     * 
     * @return arguments struct with parsed program arguments
     */
    arguments parse();
};


#endif // ARGPARSE_H