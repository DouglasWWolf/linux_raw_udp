//=============================================================================
// raw_rdmx.cpp - Class for managing raw Ethernet/IPv4/UDP/RDMX headers
//
// Author: D. Wolf
//=============================================================================
#include <cstring>
#include <cassert>
#include <arpa/inet.h>
#include "raw_rdmx.h"

#pragma pack(push, 1)

struct eth_hdr_t
{
    uint8_t     dst_mac[6];
    uint8_t     src_mac[6];
    uint16_t    frame_type;
};

struct ipv4_hdr_t
{
    uint8_t     version;
    uint8_t     dsf;
    uint16_t    length;
    uint16_t    id;
    uint16_t    flags;
    uint8_t     time_to_live;
    uint8_t     protocol;
    uint16_t    checksum;
    uint8_t     src_ip[4];
    uint8_t     dst_ip[4];
};

struct udp_hdr_t
{
    uint16_t    src_port;
    uint16_t    dst_port;
    uint16_t    length;
    uint16_t    checksum;
};

struct rdmx_hdr_t
{
    uint16_t    magic;
    uint64_t    target_addr;
    uint8_t     reserved[12];    
};

struct raw_rdmx_t
{
    eth_hdr_t   eth;
    ipv4_hdr_t  ipv4;
    udp_hdr_t   udp;
    rdmx_hdr_t  rdmx;
};

#pragma pack(pop)

//=============================================================================
// This is a 64-bit version of htonl
//=============================================================================
#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
//=============================================================================


//=============================================================================
// ipv4_checksum() - Computes the 16-bit checksum of an IPv4 header
//=============================================================================
static uint16_t ipv4_checksum(ipv4_hdr_t& header)
{
    uint16_t correction;
    uint32_t checksum = 0;

    // We're going to treat the IPv4 header as a sequence of ten
    // 16-bit big-endian integers
    uint16_t* entry = (uint16_t*)&header;

    // A standard IPv4 header is ten 16-bit integers
    for (int i=0; i<10; ++i)
    {
        // When we're computing the checksum, we don't
        // include the checksum field itself 
        if (i != 5) checksum += ntohs(entry[i]);
    }

    // Add the upper 16-bits and the lower 16-bits together
    correction = checksum >> 16;
    checksum &= 0xFFFF;
    checksum += correction;

    // An IPv4 checksum is the 1's complement of the above calculation
    return ~checksum;
}
//=============================================================================


//=============================================================================
// CRawRDMX - Default constructor
//=============================================================================
CRawRDMX::CRawRDMX()
{
    // Ensure that the structures are the expected sizes
    assert(sizeof(eth_hdr_t ) == 14);
    assert(sizeof(ipv4_hdr_t) == 20);
    assert(sizeof(udp_hdr_t ) ==  8);
    assert(sizeof(rdmx_hdr_t) == 22);
    assert(sizeof(raw_rdmx_t) == 64);

    // Set the entire frame header template to all zeros
    memset(frame_, 0, sizeof(frame_));

    // Create a handy structure reference to the frame header template
    raw_rdmx_t& frame = *(raw_rdmx_t*)frame_;

    // Set the Ethernet frame type to "IPv4"
    frame.eth.frame_type = htons(0x0800);

    // Use some reasonable default MAC addresses
    memset(frame.eth.src_mac, 0x00, 6);
    memset(frame.eth.dst_mac, 0xFF, 6);

    // Fill in the IPv4 Header
    frame.ipv4.version      = 0x45;            // 0x45 = Standard IPv4
    frame.ipv4.dsf          = 0;               // Unused
    frame.ipv4.id           = htons(0xDEAD);   // Unused
    frame.ipv4.flags        = htons(0x4000);   // Flags = "Don't fragment this packet"
    frame.ipv4.time_to_live = 0x40;            // This packet should live for 64 hops
    frame.ipv4.protocol     = 0x11;            // 0x11 = UDP 

    // The UDP checksum is always 0
    frame.udp.checksum = 0;

    // Magic number that identifies an RDMX packet
    frame.rdmx.magic = htons(0x0122);
}
//=============================================================================


//=============================================================================
// set_mac_addrs() - Defines the source and destination MAC address for the
//                   Ethernet frame
//=============================================================================
void CRawRDMX::set_mac_addrs(const void* src_mac, const void* dst_mac)
{
    unsigned char broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Create a handy structure reference to the frame header template
    raw_rdmx_t& frame = *(raw_rdmx_t*)frame_;

    // If the caller didn't give us a dst_mac, we will target this 
    // ethernet frame to the ethernet-broadcast address
    if (dst_mac == nullptr) dst_mac = broadcast_mac;

    // Copy the MAC addresses into the frame header template
    memcpy(frame.eth.src_mac, src_mac, 6);
    memcpy(frame.eth.dst_mac, dst_mac, 6);
}
//=============================================================================


//=============================================================================
// set_ip_addrs() - Defines the source and destination IP address for the
//                  IPv4 packet
//=============================================================================
void CRawRDMX::set_ip_addrs(const void* src_ip, const void* dst_ip)
{
    // Create a handy structure reference to the frame header template
    raw_rdmx_t& frame = *(raw_rdmx_t*)frame_;

    // Copy the IP addresses into the frame header template
    memcpy(frame.ipv4.src_ip, src_ip, 4);
    memcpy(frame.ipv4.dst_ip, dst_ip, 4);
}
//=============================================================================


//=============================================================================
// set_udp_ports() - Defines the source and UDP ports for the UDP datagram
//=============================================================================
void CRawRDMX::set_udp_ports(uint16_t src_port, uint16_t dst_port)
{
    // Create a handy structure reference to the frame header template
    raw_rdmx_t& frame = *(raw_rdmx_t*)frame_;

    // Store the UDP port numbers into the frame header template
    frame.udp.src_port = htons(src_port);
    frame.udp.dst_port = htons(dst_port);
}
//=============================================================================


//=============================================================================
// write_header() - Writes out a complete Ethernet/IPv4/UDP header
//=============================================================================
void CRawRDMX::write_header(void* where, uint16_t payload_length, 
                                         uint64_t target_addr)
{
    // Copy the frame header template into the caller's buffer
    memcpy(where, frame_, sizeof(frame_));    

    // Create a handy structure reference to the caller's frame header
    raw_rdmx_t& frame = *(raw_rdmx_t*)where;

    // Compute the length of the IPv4 packet
    uint16_t ip4_length = sizeof(ipv4_hdr_t)
                        + sizeof(udp_hdr_t )
                        + sizeof(rdmx_hdr_t)
                        + payload_length;
    
    // Compute the length of the UDP datagram
    uint16_t udp_length = sizeof(udp_hdr_t )
                        + sizeof(rdmx_hdr_t)
                        + payload_length;

    // Store the lengths into the caller's frame header
    frame.ipv4.length = htons(ip4_length);
    frame.udp.length  = htons(udp_length);

    // Store the IPv4 checksum into the frame header
    frame.ipv4.checksum = htons(ipv4_checksum(frame.ipv4));

    // Store the RDMX target address into the frame header
    frame.rdmx.target_addr = htonll(target_addr);
}
//=============================================================================