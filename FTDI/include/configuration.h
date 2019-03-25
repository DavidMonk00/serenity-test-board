#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif

/* mpasse */
#include <mpsse.h>

#ifdef __cplusplus
}
#endif

#include "consts.h"
#include "I2CIO.h"

int selectI2Cline( struct mpsse_context *i2c, uint32_t line );
int select_MUX_GND_channel( struct mpsse_context *i2c, uint32_t ch );
int select_MUX_ANALOG_channel( struct mpsse_context *i2c, uint32_t muxID, uint32_t ch );
int config( struct mpsse_context *i2c, int gndMuxCH, int analogMuxId, int analogMuxCh );
