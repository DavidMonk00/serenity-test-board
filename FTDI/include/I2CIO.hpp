#pragma once

#include "consts.hpp"
#include <stdio.h>

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
    int write(uint32_t addr, uint32_t data);
    int read(uint32_t addr, uint32_t *data, uint32_t ndata );
};
