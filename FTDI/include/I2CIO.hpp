#pragma once

#include "consts.hpp"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <ftdi.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

// #include "mpsse.h"

extern "C" {
    #include <mpsse.h>
}

class I2CIO {
private:
    struct mpsse_context *mpsse;
    struct mpsse_context *mpsse_c;


public:
    I2CIO();
    virtual ~I2CIO();
    int getStatus();
    int send(uint8_t addr, const std::vector<uint8_t>& write_data,
             std::vector<uint8_t>& read_data);
    int write(uint8_t addr, const std::vector<uint8_t>& write_data);
    int read(uint8_t addr, std::vector<uint8_t>& read_data);
};
