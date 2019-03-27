#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "consts.hpp"
#include "FTDI.hpp"
//#include "I2CIO.hpp"

int selectI2Cline(FTDI* i2c, uint32_t line );
int select_MUX_GND_channel(FTDI* i2c, uint32_t ch );
int select_MUX_ANALOG_channel(FTDI* i2c, uint32_t muxID, uint32_t ch );
int config(FTDI* i2c, int gndMuxCH, int analogMuxId, int analogMuxCh );
