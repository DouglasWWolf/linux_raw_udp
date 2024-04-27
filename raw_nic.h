//=============================================================================
// This class sends raw ethernet frames to a NIC using the raw sockets API
//
// Author: D. Wolf
//=============================================================================
#pragma once
#include <cstdint>

class CRawNIC
{

public:

    void    connect_nic(const char* nic_name);

    // If frame_length is more than 1500 bytes, make sure the MTU of 
    // your NIC is set to a large enough value!
    void    send(const void* frame, uint16_t frame_length);

protected:

    // Socket descriptor
    int     m_sd;
    
    // Network interface index
    int     m_if_idx;
};