#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>

#include "consts.hpp"
#include "I2CIO.hpp"
#include "I2C_serial.hpp"

extern "C" {
    #include <mpsse.h>
}



int selectI2Cline(I2C_serial* i2c, uint32_t line );
int select_MUX_GND_channel(I2C_serial* i2c, uint32_t ch );
int select_MUX_ANALOG_channel(I2C_serial* i2c, uint32_t muxID, uint32_t ch );
int config(I2C_serial* i2c, int gndMuxCH, int analogMuxId, int analogMuxCh );
