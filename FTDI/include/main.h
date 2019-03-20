//#pragma once

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>

#include "consts.h"
#include "I2CIO.h"
#include "export.h"
#include "configuration.h"

/* mpasse */
#include <mpsse.h>

float readADC( struct mpsse_context *i2c );
int loopOverChannels(struct mpsse_context *i2c, int nPoints, char* dataBuf);
int singleReading(struct mpsse_context *i2c, char* mux_label, int nPoints);
int loopOverPP( struct mpsse_context *i2c, int nPoints, struct channel_reading* data);
