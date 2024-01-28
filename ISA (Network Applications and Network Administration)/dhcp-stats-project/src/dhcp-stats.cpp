/**
 * @file dhcp-stats.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief main of dhcp-stats tool
 * @date 2023-10-19
 * 
 */


#include "dhcp-stats.hpp"


using namespace std;


int main(int argc, char *argv[])
{
    ArgParser ArgumentParser(argc, argv);
    arguments args = ArgumentParser.parse();

    DHCPMonitor monitor(args.ip_prefixes, args.interface, args.pcap_file, args.leaseCheckSec);
    monitor.analyze();

    return 0;
}

/* END OF FILE */