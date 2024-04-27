#pragma once
#include <cstdint>

class CRawNIC
{

public:

    void    connect_nic(const char* nic_name);

    void    send(const void* frame, uint16_t frame_length);

protected:

    // Socket descriptor
    int     m_sd;
    
    // Network interface index
    int     m_if_idx;
};