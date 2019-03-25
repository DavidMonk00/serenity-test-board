#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "consts.h"
#include "I2CIO.h"
#include "configuration.h"

/* mpasse */
#include <mpsse.h>

float readADC( struct mpsse_context *i2c );
struct channel_reading readChannel(struct mpsse_context *i2c, int nPoints, int imux, int ich);
int loopOverChannels(struct mpsse_context *i2c, int nPoints, char* dataBuf);
int singleReading(struct mpsse_context *i2c, char* mux_label, int nPoints);
