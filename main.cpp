
// Author: D. Wolf
#include <stdio.h>
#include <cstdint>
#include "raw_nic.h"
#include "raw_udp.h"
#include "raw_rdmx.h"


//=============================================================================
// Some constants that we'll use in our examples
//=============================================================================
#define UDP_HEADER_SIZE  42
#define RDMX_HEADER_SIZE 64
#define PAYLOAD_LEN      256
//=============================================================================

// Provides raw Ethernet access to the NIC
CRawNIC  NIC;

// Creates headers for Ethernet/IPv4/UDP frames
CRawUDP  udp_frame_header;

// Creates headers for Ethernet/IPv4/UDP/RDMX frames
CRawRDMX rdmx_frame_header;

// Just a big buffer to create frames in for our demonstration
uint8_t ethernet_frame[10000];

//=============================================================================
// Function prototypes
//=============================================================================
void make_payload(uint8_t* where, uint16_t length);
void demonstrate_udp_frame();
void demonstrate_rdmx_frame();
//=============================================================================


//=============================================================================
// main() - Execution begins here
//=============================================================================
int main()
{
    // Create a raw connection to our network interface
    NIC.connect_nic("enp3s0");

    // Send a UDP packet 
    demonstrate_udp_frame();

    // Send an RDMX packet
    demonstrate_rdmx_frame();
}
//=============================================================================


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
void demonstrate_udp_frame()
{
    // MAC address and IP addresses we want to use in our Ethernet frame
    uint8_t src_mac[] = {0xC4, 0x00, 0xAD, 0x3A, 0xD3, 0x6B};
    uint8_t src_ip[]  = {10, 11, 12, 1};
    uint8_t dst_ip[]  = {10, 11, 12, 255};
    uint16_t src_port = 1234;
    uint16_t dst_port = 5678;

    // Tell the frame-header template about:
    //  (1) Our MAC addresses
    //  (2) Our IP addresses
    //  (3) Our UDP ports
    udp_frame_header.set_mac_addrs(src_mac);
    udp_frame_header.set_ip_addrs(src_ip, dst_ip);
    udp_frame_header.set_udp_ports(src_port, dst_port);

    // Stamp an Ethernet/IPv4/UDP header into 'ethernet_frame'
    udp_frame_header.write_header(ethernet_frame, PAYLOAD_LEN);

    // For demo purposes, stuff a simple payload into 'ethernet_frame'
    make_payload(ethernet_frame + UDP_HEADER_SIZE, PAYLOAD_LEN);

    // Transmit the Ethernet frame that we built from scratch
    NIC.send(ethernet_frame, UDP_HEADER_SIZE + PAYLOAD_LEN);
}
//=============================================================================




//=============================================================================
// This demonstrates how to create an RDMX frame-header template, and how to 
// use it to write a complete Ethernet/IPv4/UDP/RDMX frame-header to a buffer
//=============================================================================
void demonstrate_rdmx_frame()
{
    
    // MAC address and IP addresses we want to use in our Ethernet frame
    uint8_t src_mac[] = {0xC4, 0x00, 0xAD, 0x3A, 0xD3, 0x6B};
    uint8_t src_ip[]  = {10, 11, 12, 1};
    uint8_t dst_ip[]  = {10, 11, 12, 255};
    uint16_t src_port = 1234;

    // Tell the frame-header template about:
    //  (1) Our MAC addresses
    //  (2) Our IP addresses
    //  (3) Our UDP ports
    rdmx_frame_header.set_mac_addrs(src_mac);
    rdmx_frame_header.set_ip_addrs(src_ip, dst_ip);
    rdmx_frame_header.set_udp_ports(src_port);

    // This is the RDMX target address where the receiver will store the packet
    const uint64_t TARGET_ADDRESS = 0x123456789abcdef0LL;
 
    // Stamp an Ethernet/IPv4/UDP/RDMX header into 'ethernet_frame'
    rdmx_frame_header.write_header(ethernet_frame, PAYLOAD_LEN, TARGET_ADDRESS);

    // For demo purposes, stuff a simple payload into 'ethernet_frame'
    make_payload(ethernet_frame + RDMX_HEADER_SIZE, PAYLOAD_LEN);

    // Transmit the Ethernet frame that we built from scratch
    NIC.send(ethernet_frame, RDMX_HEADER_SIZE + PAYLOAD_LEN);
}
//=============================================================================



