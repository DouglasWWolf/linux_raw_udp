//=============================================================================
// This class sends raw ethernet frames to a NIC using the raw sockets API
//
// Author: D. Wolf
//=============================================================================
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include "raw_nic.h"

//=============================================================================
// dump() - Hex-dump a memory region
//=============================================================================
#if 0
static void dump(void* data, int length)
{

    int n=0;

    // Get a handy pointer to the data
    const unsigned char* p = (const unsigned char*)data;

    while (length--)
    {
        printf("%02X ", *p++);

        ++n;
        if (n%8 == 0) printf("  ");
        if (n%16 == 0) printf("\n");

    }
    printf("\n");

}
#endif
//=============================================================================


//=============================================================================
// connect_nic() - Opens the raw socket and fetches the index of the specific
//                 network interface
//=============================================================================
void CRawNIC::connect_nic(const char* nic_name)
{
    // Open raw socket to send on
    m_sd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

    // If the open failed, barf
    if (m_sd == -1)
    {
        perror("socket");
        exit(1);
    }

    struct ifreq if_data;

    // Clear the structure that will hold our network interface information
    memset(&if_data, 0, sizeof(if_data));
	
    // Stuff the network interface name into the network interface info structure
    strncpy(if_data.ifr_name, nic_name, IFNAMSIZ-1);
	
    // Fetch information about this network interface
    if (ioctl(m_sd, SIOCGIFINDEX, &if_data) < 0)
    {
        perror("SIOCGIFINDEX");
        exit(1);
    }

    // And save the index of the user-specified network interface
    m_if_idx = if_data.ifr_ifindex;
}
//=============================================================================


//=============================================================================
// send() - Transmits a raw ethernet frame over the network interface
//=============================================================================
void CRawNIC::send(const void* frame, uint16_t frame_length)
{
    struct sockaddr_ll socket_address;

    // Fill in the interface index of the socket address
    socket_address.sll_ifindex = m_if_idx;

    // Fill in the length of the destination MAC
    socket_address.sll_halen = 6;

    // Fill in the destination MAC
    memcpy(socket_address.sll_addr, frame, 6);

    // Send the packet to the network interface
    int rc = sendto(m_sd, frame, frame_length, 0, (sockaddr*)&socket_address,
                                                   sizeof(socket_address));
    if (rc < 1)
    {
        printf("sendto failed\n");        
        perror("sendto:");      
    }
}
//=============================================================================
