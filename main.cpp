#include <stdio.h>
#include <cstdint>
#include "raw_nic.h"
#include "raw_udp.h"

CRawNIC NIC;
CRawUDP udp_frame_header;
uint8_t ethernet_frame[10000];

#define UDP_HEADER_SIZE 42
#define PAYLOAD_LEN 256

//=============================================================================
// make_payload() - This writes a very simple mock "payload" into a buffer
//=============================================================================
void make_payload(uint8_t* where, uint16_t length)
{
    for (int i=0; i<length; ++i) *where++ = i;
}
//=============================================================================


//=============================================================================
// This demonstrates how to create a UDP frame-header template, and how to 
// use it to write a complete Ethernet/IPv4/UDP frame-header to a buffer
//=============================================================================
void demonstrate_send_udp_frame()
{
    // MAC address and IP addresses we want to use in our Ether frame
    uint8_t src_mac[] = {0xC4, 0x00, 0xAD, 0x3A, 0xD3, 0x6B};
    uint8_t src_ip[]  = {10, 11, 12, 1};
    uint8_t dst_ip[]  = {10, 11, 12, 255};

    // Tell the frame-header template about:
    //  (1) Our MAC addresses
    //  (2) Our IP addresses
    //  (3) Our UDP ports
    udp_frame_header.set_mac_addrs(src_mac);
    udp_frame_header.set_ip_addrs(src_ip, dst_ip);
    udp_frame_header.set_udp_ports(1234, 5678);

    // Stamp an Ethernet/IPv4/UDP header into 'ethernet_frame'
    udp_frame_header.write_header(ethernet_frame, PAYLOAD_LEN);

    // For demo purposes, stuff simple payload into 'ethernet_frame'
    make_payload(ethernet_frame + UDP_HEADER_SIZE, PAYLOAD_LEN);

    // Transmit the Ethernet frame that we build from scratch
    NIC.send(ethernet_frame, UDP_HEADER_SIZE + PAYLOAD_LEN);
}
//=============================================================================



//=============================================================================
// main() - Execution begins here
//=============================================================================
int main()
{
    // Create a raw connection to our network interface
    NIC.connect_nic("enp3s0");

    // Send a UDP packet 
    demonstrate_send_udp_frame();
}
//=============================================================================