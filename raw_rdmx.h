//=============================================================================
// raw_rdmx.h - Class for managing raw Ethernet/IPv4/UDP/RDMX headers
//
// Author: D. Wolf
//
// To use this class:
//
// (1) declare an instance of "CRawRDMX"
//
// (2) call the three "set_XXX" functions to fill in the details
//
// (3) call "write_header()" to write the 64-byte Ethernet/IPv4/UDP/RDMX
//     frame header at your desired location.
//=============================================================================
#pragma once
#include <cstdint>

class CRawRDMX
{
public:

    // Constructer fills in header field constants
    CRawRDMX();

    // Call this to define source and destination MAC addresses.  If dst_mac
    // is "nullptr", it will be set to the broadcoast MAC (FF:FF:FF:FF:FF:FF)
    void    set_mac_addrs(const void* src_mac, const void* dst_mac = nullptr);

    // Call this to define the source and destination IP addresses.
    void    set_ip_addrs(const void* src_ip, const void* dst_ip);

    // Call this to define the source and destination UDP ports
    void    set_udp_ports(uint16_t src_port, uint16_t dst_port = 11111);

    // Call this to write out a valid Ethernet/IPv4/UDP/RDMX header
    void    write_header(void* where, uint16_t payload_length, uint64_t target_addr);

protected:

    // This will contain the template for the Ethernet/IPv4/UDP frame
    unsigned char frame_[64];
};

