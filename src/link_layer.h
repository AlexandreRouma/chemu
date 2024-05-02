#pragma once
#include <stdint.h>

#define CODAN_PHY_ADDR_HANDSET  0x05
#define CODAN_PHY_ADDR_RF_UNIT  0x09

#define CODAN_LINK_ADDR_HANDSET  0xFA
#define CODAN_LINK_ADDR_RF_UNIT  0xF6

namespace CODAN {
    class LinkLayer {
    public:
        int send(uint8_t phyAddr, uint8_t linkAddr, uint8_t* data, int len);

    private:
        void worker();
    };
}