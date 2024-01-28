
import os
os.sys.path.append('/home/miski/.local/lib/python3.10/site-packages')

from scapy.all import Ether, IP, UDP, BOOTP, DHCP, wrpcap, sendp
import random
import time

INTERFACE = "lo"

SERVER_MAC = "08:00:2B:2E:AF:2A"
SERVER_IP = "192.168.1.0"


def generate_mac_addresses(N):
    mac_addresses = set()
    while len(mac_addresses) < N:
        # Generate 6 random hexadecimal values and join them with colons
        mac_address = ':'.join(['{:02x}'.format(random.randint(0, 255)) for _ in range(6)])
        mac_addresses.add(mac_address)
    return list(mac_addresses)



ADDRESS_1_COUNT = 10 # 192.168.0.0/16
ADDRESS_2_COUNT = 10 # 172.16.0.0/16
ADDRESS_3_COUNT = 50 # 10.0.0.0/8
ADDRESS_COUNT = ADDRESS_1_COUNT + ADDRESS_2_COUNT + ADDRESS_3_COUNT
CLIENTS_MAC = generate_mac_addresses(ADDRESS_COUNT)
CLIENTS_IP = ["192.168.1."+str(i) for i in range(0,ADDRESS_1_COUNT)] + ["172.16.1."+str(i) for i in range(0,ADDRESS_2_COUNT)] + ["10.0.0."+str(i) for i in range(0,ADDRESS_3_COUNT)]

# ####################
# Test on ./bin/dhcp-stats -i lo 192.168.1.0/24 172.16.1.0/28 10.0.0.0/16
# ####################


# Function to create a DHCP Discover packet
def create_dhcp_discover(client_mac):
    dhcp_discover = Ether(src=client_mac, dst="ff:ff:ff:ff:ff:ff") / IP(src="0.0.0.0", dst="255.255.255.255") / UDP(sport=68, dport=67) / BOOTP(chaddr=client_mac) / DHCP(options=[("message-type", "discover"), "end"])
    return dhcp_discover


# Function to create a DHCP Offer packet
def create_dhcp_offer(client_mac, offered_ip):
    dhcp_offer = Ether(src=SERVER_MAC, dst=client_mac) / IP(src=SERVER_IP, dst="255.255.255.255") / UDP(sport=67, dport=68) / BOOTP(op=2, chaddr=client_mac, yiaddr=offered_ip) / DHCP(options=[("message-type", "offer"), ("server_id", SERVER_IP), "end"])
    return dhcp_offer


# Function to create a DHCP Request packet
def create_dhcp_request(client_mac, requested_ip, server_mac="ff:ff:ff:ff:ff:ff", client_ip="0.0.0.0", server_ip="255.255.255.255"):
    dhcp_request = Ether(src=client_mac, dst=server_mac) / IP(src=client_ip, dst=server_ip) / UDP(sport=68, dport=67) / BOOTP(chaddr=client_mac) / DHCP(options=[("message-type", "request"), ("requested_addr", requested_ip), "end"])
    return dhcp_request


# Function to create a DHCP Acknowledgement packet
def create_dhcp_ack(client_mac, leased_ip, lease_time=99999):
    dhcp_ack = Ether(src=SERVER_MAC, dst=client_mac) / IP(src=SERVER_IP, dst="255.255.255.255") / UDP(sport=67, dport=68) / BOOTP(op=2, chaddr=client_mac, yiaddr=leased_ip) / DHCP(options=[("message-type", "ack"), ("server_id", SERVER_IP), ("lease_time", lease_time), "end"])
    return dhcp_ack


# Function to create a DHCP Acknowledgement packet
def create_dhcp_ack_no_lease(client_mac, leased_ip):
    dhcp_ack = Ether(src=SERVER_MAC, dst=client_mac) / IP(src=SERVER_IP, dst="255.255.255.255") / UDP(sport=67, dport=68) / BOOTP(op=2, chaddr=client_mac, yiaddr=leased_ip) / DHCP(options=[("message-type", "ack"), ("server_id", SERVER_IP), "end"])
    return dhcp_ack


# Function to create a DHCP Release packet
def create_dhcp_release(client_mac, client_ip):
    dhcp_release = Ether(src=client_mac, dst=SERVER_MAC) / IP(src=client_ip, dst=SERVER_IP) / UDP(sport=68, dport=67) / BOOTP(chaddr=client_mac, ciaddr=client_ip, flags=1) / DHCP(options=[("message-type", "release"), "end"])
    return dhcp_release


# Function to create a DHCP NAK packet
def create_dhcp_nak(client_mac, client_ip="255.255.255.255"):
    dhcp_nak = Ether(src=SERVER_MAC, dst=client_mac) / IP(src=SERVER_IP, dst=client_ip) / UDP(sport=67, dport=68) / BOOTP(op=2, chaddr=client_mac) / DHCP(options=[("message-type", "nak"), "end"])
    return dhcp_nak


# Function to create a DHCP Decline packet
def create_dhcp_decline(client_mac, client_ip):
    dhcp_decline = Ether(src=client_mac, dst=SERVER_MAC) / IP(src=client_ip, dst=SERVER_IP) / UDP(sport=67, dport=68) / BOOTP(op=2, ciaddr=client_ip, chaddr=client_mac) / DHCP(options=[("message-type", "decline"), "end"])
    return dhcp_decline




# Test normal dhcp traffic : DISCOVER -> OFFER -> REQUEST -> ACK
def test_normal_dhcp_traffic(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

    sendp(packets, interface)
    return packets


# Test dhcp decline : DISCOVER -> OFFER -> REQUEST -> ACK -> DECLINE
def test_dhcp_decline(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

        # decline addresses from 9-21
        if i > 8 and i < 22:
            packets.append(create_dhcp_decline(CLIENTS_MAC[i], CLIENTS_IP[i]))

    sendp(packets, interface)
    return packets


# Test dhcp negative ack : DISCOVER -> OFFER -> REQUEST -> NAK, DISCOVER -> OFFER -> REQUEST -> ACK -> REQUEST -> NAK
def test_dhcp_nak(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))

        # decline addresses from 9-21
        if i > 8 and i < 22:
            packets.append(create_dhcp_nak(CLIENTS_MAC[i]))
        else:
            packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

    sendp(packets, interface)
    return packets


# Test dhcp release : DISCOVER -> OFFER -> REQUEST -> ACK -> RELEASE
def test_dhcp_release(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

        # decline addresses from 9-21
        if i > 8 and i < 22:
            packets.append(create_dhcp_release(CLIENTS_MAC[i], CLIENTS_IP[i]))
            
    sendp(packets, interface)
    return packets


# Test double ack : DISCOVER -> OFFER -> REQUEST -> ACK -> ACK
def test_dhcp_double_ack(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

        # decline addresses from 9-21
        if i > 8 and i < 22:
            packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))
            
    sendp(packets, interface)
    return packets


# Test double ack : DISCOVER -> OFFER -> REQUEST -> ACK -> ACK
def test_dhcp_no_lease_time_ack(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        # decline addresses from 9-21
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        if i > 8 and i < 22:
            packets.append(create_dhcp_ack_no_lease(CLIENTS_MAC[i], CLIENTS_IP[i]))
        else:
            packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

            
    sendp(packets, interface)
    return packets


# Test dhcp lease time expiration : DISCOVER -> OFFER -> REQUEST -> ACK -> ...
def test_dhcp_lease_time(interface=INTERFACE):
    packets = []
    for i in range(0,ADDRESS_COUNT):
        packets.append(create_dhcp_discover(CLIENTS_MAC[i]))
        packets.append(create_dhcp_offer(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_request(CLIENTS_MAC[i], CLIENTS_IP[i]))
        packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i]))

        # decline addresses from 9-21
        if i > 8 and i < 22:
            packets.append(create_dhcp_ack(CLIENTS_MAC[i], CLIENTS_IP[i], lease_time=10))
            
    sendp(packets, interface)
    return packets


if __name__ == "__main__":
    #packets = test_normal_dhcp_traffic()
    #packets = test_dhcp_decline()
    #packets = test_dhcp_nak()
    #packets = test_dhcp_release()
    #packets = test_dhcp_double_ack()
    #packets = test_dhcp_no_lease_time_ack()
    packets = test_dhcp_lease_time()

    #wrpcap('dhcp-double-ack.pcap', packets)