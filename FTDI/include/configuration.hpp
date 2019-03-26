#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "consts.hpp"
#include "I2CIO.hpp"

int selectI2Cline(I2CIO* i2c, uint32_t line );
int select_MUX_GND_channel(I2CIO* i2c, uint32_t ch );
int select_MUX_ANALOG_channel(I2CIO* i2c, uint32_t muxID, uint32_t ch );
int config(I2CIO* i2c, int gndMuxCH, int analogMuxId, int analogMuxCh );
