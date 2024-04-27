//=============================================================================
// raw_udp.h - Class for managing raw Ethernet/IPv4/UDP headers
//
// To use this class:
//
// (1) declare an instance of "CRawUDP"
//
// (2) call the three "set_XXX" functions to fill in the details
//
// (3) call "write_header()" to write the 42-byte Ethernet/IPv4/UDP frame
//     header at your desired location.
//=============================================================================
#pragma once
#include <cstdint>

class CRawUDP
{
public:

    // Constructer fills in header field constants
    CRawUDP();

    // Call this to define source and destination MAC addresses.  If dst_mac
    // is "nullptr", it will be set to the broadcoast MAC (FF:FF:FF:FF:FF:FF)
    void    set_mac_addrs(void* src_mac, void* dst_mac = nullptr);

    // Call this to define the source and destination IP addresses.
    void    set_ip_addrs(void* src_ip, void* dst_ip);

    // Call this to define the source and destination UDP ports
    void    set_udp_ports(uint16_t src_port, uint16_t dst_port);

    // Call this to write out a valid Ethernet/IPv4/UDP header
    void    write_header(void* where, uint16_t payload_length);

protected:

    // This will contain the template for the Ethernet/IPv4/UDP frame
    unsigned char frame_[42];
};


