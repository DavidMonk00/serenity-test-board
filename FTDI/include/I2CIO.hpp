#pragma once

#include "consts.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <ftdi.h>

// #include "mpsse.h"

extern "C" {
    #include <mpsse.h>
}

class I2CIO {
private:
    struct mpsse_context *m_context;

public:
    I2CIO();
    virtual ~I2CIO();
    int getStatus();
    int send(uint8_t addr, const std::vector<uint8_t>& write_data,
             std::vector<uint8_t>& read_data);
    int write(uint32_t addr, uint32_t data);
    int read(uint32_t addr, std::vector<uint8_t>& data, uint32_t ndata=1);
};
