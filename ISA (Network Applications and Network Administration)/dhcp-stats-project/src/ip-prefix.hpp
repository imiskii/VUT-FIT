/**
 * @file ip-prefix.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Header file for ip-prefix.cpp
 * @date 2023-10-19
 * 
 */


#include <string>
#include <cstdint>
#include <regex>

#include "errors.hpp"


#ifndef IP_PREFIX_H
#define IP_PREFIX_H


using namespace std;


/******************** MACROS ********************/

#define IPv4_OCTET_SIZE 8       /// size of IPv4 octet
#define IPv4_SIZE 32            /// IPv4 size in bits 
#define IPv4_OCTET_MASK 0xFF    /// Mask for first 8 bits of IPv4 address


/*********** FUNCTIONS AND CLASSES **************/


class IPv4Prefix
{
private:
    /* Private Attributes */
    string _IPprefix;           // name of prefix
    uint32_t _mask;             // 32bit unsigned representing mask of the prefix
    uint32_t _firstIP;          // 32bit unsigned representing first ip in given range (usually gateway address)
    uint32_t _lastIP;           // 32bit unsigned representing last ip in given range (usually broadcast address)
    unsigned _maxHosts;         // max ip address in given range
    unsigned _allocatedHosts;   // current number of allocated addresses

    /* Private Methods */


public:
    /* Public Attributes */


    /* Public Methods */

    IPv4Prefix(string IPWithPrefix);
    ~IPv4Prefix();

    /**
     * @brief Convert IPv4 address to 32 bits long unsigned integer 
     * 
     * @param ipAddress IPv4 address that will be transformed to unsigned integer
     * @return uint32_t 
     */
    static uint32_t IP2UINT(const string &ipAddress);


    /**
     * @brief Convert 32 bits long integer to IPv4 address
     * 
     * @param ipAddress unsigned integer that will be converted
     * @return string 
     */
    static string UINT2IP(const uint32_t &ipAddress);

    /**
     * @brief Check if given string is valid IPv4 address
     * 
     * @param ipAddress  IPv4 address
     * @return true if is valid IPv4 address
     * @return false  if is not valid IPv4 address
     */
    static bool isIPv4(const string &ipAddress);

    /**
     * @brief increase allocated host count if given address is from prefix range
     * 
     * @param ipAddress 
     */
    void addAddress(const string &ipAddress);
    void addAddress(const uint32_t &ipAddress);

    /**
     * @brief decrease allocated host count if given address is from prefix range
     * 
     * @param ipAddress 
     */
    void removeAddress(const string &ipAddress);
    void removeAddress(const uint32_t &ipAddress);

    /**
     * @brief Checks if given ip address is in prefix range
     * It gives FALSE to first (gateway) and last (broadcast) address in ip range
     * 
     * @param ipAddress IPv4 address
     * @return true if is in range
     * @return false if is not in range
     */
    bool isInPrefix(const string &ipAddress);
    bool isInPrefix(const uint32_t &ipAddress);

    /**
     * @brief count percentage of allocated ip addresses
     * 
     * @return double percentage of allocated ip addresses
     */
    double calculateUtilizationPercentage();

    /**
     * @brief Get the Max Hosts object
     * 
     * @return unsigned max host that can be allocated in this prefix
     */
    unsigned getMaxHosts();

    /**
     * @brief Get the Allocated Hosts object
     * 
     * @return unsigned current count of allocated host in this prefix
     */
    unsigned getAllocatedHosts();

    /**
     * @brief Get the Prefix Name object
     * 
     * @return string prefix name
     */
    string getPrefixName();
};



#endif // IP_PREFIX_H