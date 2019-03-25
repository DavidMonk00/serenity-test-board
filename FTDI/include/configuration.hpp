#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>

#include "consts.hpp"

#ifndef _LIBMPSSE_
#define _LIBMPSSE_

extern "C" {
    #include <mpsse.h>
}

#endif /* end of include guard: _LIBMPSSE_ */

#include "I2CIO.hpp"

int selectI2Cline( struct mpsse_context *i2c, uint32_t line );
int select_MUX_GND_channel( struct mpsse_context *i2c, uint32_t ch );
int select_MUX_ANALOG_channel( struct mpsse_context *i2c, uint32_t muxID, uint32_t ch );
int config( struct mpsse_context *i2c, int gndMuxCH, int analogMuxId, int analogMuxCh );
