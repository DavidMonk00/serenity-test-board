#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "consts.hpp"
#include "component.hpp"
#include "I2CIO.hpp"

extern "C" {
    #include <mpsse.h>
}


class FT2232H : Component {
private:
    struct mpsse_context *i2c;

public:
    FT2232H (void);
    virtual ~FT2232H (void);
    float readADC(void);
};
