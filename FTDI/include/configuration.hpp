#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>

#include "consts.hpp"
#include "FT2232H.hpp"

void selectI2Cline(FT2232H* ftdi, uint32_t line );
void select_MUX_GND_channel(FT2232H* ftdi, uint32_t ch );
void select_MUX_ANALOG_channel(FT2232H* ftdi, uint32_t muxID, uint32_t ch );
void config(FT2232H* ftdi, int gndMuxCH, int analogMuxId, int analogMuxCh );
