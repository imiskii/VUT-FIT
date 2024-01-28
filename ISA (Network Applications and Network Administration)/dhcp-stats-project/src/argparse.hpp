/**
 * @file argparse.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Header file for argparse.cpp
 * @date 2023-10-19
 * 
 */


#include <getopt.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <tuple>
#include <regex>
#include <iostream>
#include <filesystem>

#include <pcap/pcap.h>

#include "ip-prefix.hpp"
#include "errors.hpp"

#ifndef ARGPARSE_H
#define ARGPARSE_H


using namespace std;


/******************** MACROS ********************/

#define DEFAULT_LEASE_TIME_CHECK 60 

/****************** DATATYPES *******************/


// parsed program arguments
struct arguments
{
    string interface;               /// interface
    string pcap_file;               /// path to pcap file
    vector<IPv4Prefix*> ip_prefixes; /// IP_V4 prefixes
    time_t leaseCheckSec;           /// frequency of lease time check in seconds
};


// argument flags (tells if program argument was given)
struct arg_flags  
{
    bool h_flag;    /// help flag
    bool i_flag;    /// interface flag
    bool r_flag;    /// file flag
    bool p_flag;    /// prefix flag
    bool t_flag;    /// lease check time flag
};

/*********** FUNCTIONS AND CLASSES **************/


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

    /**
     * @brief Check if given IPv4 address prefix is valid
     * 
     * @param address ip address
     * @return true if it is valid IPv4 address
     * @return false if it is not valid IPv4 address
     */
    bool isValidIPv4Prefix(const char *address);

    bool isValidPcapFile(const char *pcap_file_path);

};



#endif // ARGPARSE_H