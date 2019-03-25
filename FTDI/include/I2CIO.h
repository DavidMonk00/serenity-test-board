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


int I2Cwrite( struct mpsse_context *i2c, uint32_t addr, uint32_t data);
int I2Cread( struct mpsse_context *i2c, uint32_t addr, uint32_t *data, uint32_t ndata );
