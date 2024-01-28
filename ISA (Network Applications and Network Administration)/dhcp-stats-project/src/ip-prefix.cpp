/**
 * @file ip-IPWithPrefix.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Classes representing IP prefixes
 * @date 2023-10-19
 * 
 */


#include "ip-prefix.hpp"


IPv4Prefix::IPv4Prefix(string IPWithPrefix) : _IPprefix(IPWithPrefix)
{
    // find the prefix (the check are not needed because it was previously chacked with regex)
    size_t slash_index = this->_IPprefix.find('/');
    int prefix = stoi(this->_IPprefix.substr(slash_index+1));
    
    // make a mask for prefix
    this->_mask = (~0U) << (IPv4_SIZE - prefix);

    // find the lowest (start) and highest (end) ip addres in range given by prefix
    uint32_t u_ip = IP2UINT(this->_IPprefix.substr(0, slash_index));
    this->_firstIP = u_ip & this->_mask;
    this->_lastIP = this->_firstIP | (~this->_mask);

    this->_allocatedHosts = 0;
    this->_maxHosts = (this->_lastIP - this->_firstIP) - 1;
}


IPv4Prefix::~IPv4Prefix()
{

}


uint32_t IPv4Prefix::IP2UINT(const string &ipAddress)
{
    uint32_t result = 0;
    istringstream iss(ipAddress);
    string octet;

    if (isIPv4(ipAddress))
    {
        for (short shift = (IPv4_SIZE-IPv4_OCTET_SIZE); shift >= 0; shift -= IPv4_OCTET_SIZE)
        {
            getline(iss, octet, '.');
            result = result | (static_cast<uint32_t>(stoul(octet)) << shift);
        }
    }
    else
    {
        print_warning("Function IP2UINT got invalid IPv4 address: '", ipAddress, "' !");
    }

    return result;
}


string IPv4Prefix::UINT2IP(const uint32_t &ipAddress)
{
    string result = "";
    
    for (short shift = (IPv4_SIZE-IPv4_OCTET_SIZE); shift >= 0; shift -= IPv4_OCTET_SIZE)
    {
        result += to_string((ipAddress >> shift) & IPv4_OCTET_MASK);
        if (shift != 0)
        {
            result += '.';
        }
    }

    return result;
}


bool IPv4Prefix::isIPv4(const string &ipAddress)
{
    regex ipv4("\\s*^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\s*$");
    return regex_match(ipAddress, ipv4);
}


void IPv4Prefix::addAddress(const string &ipAddress)
{
    // Check if given ip address is in prefix range
    if (isInPrefix(ipAddress))
    {
        this->_allocatedHosts++;
    }
}

void IPv4Prefix::addAddress(const uint32_t &ipAddress)
{
    // Check if given ip address is in prefix range
    if (isInPrefix(ipAddress))
    {
        this->_allocatedHosts++;
    }
}

void IPv4Prefix::removeAddress(const string &ipAddress)
{
    // Check if given ip address is in prefix range
    if (isInPrefix(ipAddress))
    {
        this->_allocatedHosts--;
    }
}

void IPv4Prefix::removeAddress(const uint32_t &ipAddress)
{
    // Check if given ip address is in prefix range
    if (isInPrefix(ipAddress))
    {
        this->_allocatedHosts--;
    }
}

bool IPv4Prefix::isInPrefix(const string &ipAddress)
{
    uint32_t uint_ip = IP2UINT(ipAddress);
    if (this->_firstIP < uint_ip && this->_lastIP > uint_ip)
    {
        return true;
    }
    return false;
}

bool IPv4Prefix::isInPrefix(const uint32_t &ipAddress)
{
    if (this->_firstIP < ipAddress && this->_lastIP > ipAddress)
    {
        return true;
    }
    return false;
}

double IPv4Prefix::calculateUtilizationPercentage()
{
    return (static_cast<double>(this->_allocatedHosts)/this->_maxHosts)*100;
}


unsigned IPv4Prefix::getMaxHosts()
{
    return this->_maxHosts;
}


unsigned IPv4Prefix::getAllocatedHosts()
{
    return this->_allocatedHosts;
}


string IPv4Prefix::getPrefixName()
{
    return this->_IPprefix;
}
