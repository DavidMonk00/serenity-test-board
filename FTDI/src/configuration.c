#include "configuration.h"

int selectI2Cline( struct mpsse_context *i2c, uint32_t line ) {

    int i = 0;
    for( ; i<10; i++ )
        if( I2Cwrite( i2c, I2C_MUX_ADDR, line) >= 0 )
            return 0;

    return -1;

}


int select_MUX_GND_channel( struct mpsse_context *i2c, uint32_t ch ){

    if( ch<0 || ch>7 ) {
        printf("Ground MUX channel must be in the range [0,7] ");
        return -1;
    }

    selectI2Cline( i2c, MUX_GND_I2C_LINE );

    uint32_t data = ( MUX_EN | MUX_CH[ch] );

    int i=0;
    for( ; i<10; i++ )
        if( I2Cwrite( i2c, MUX_GND_ADDR, data) >= 0 )
            return 0;

    return -1;

}


int select_MUX_ANALOG_channel( struct mpsse_context *i2c, uint32_t muxID, uint32_t ch ){

    if( ch<0 || ch>7 ) {
        printf("Analog MUX channel must be in the range [0,7] ");
        return -1;
    }

    if( muxID<0 || muxID>3 ) {
        printf("Analog MUX ID must be in the range [0,3] ");
        return -1;
    }

    selectI2Cline( i2c, MUX_ANALOG_I2C_LINE );

    /* disable all outputs */

    int i=0, imux=0;

    for( ; imux<4; imux++ )
        for( ; i<10; i++ )
            if( I2Cwrite( i2c, MUX_ANALOG_ADDR[imux], 0) >= 0 )
                break;

    uint32_t data = ( MUX_EN | MUX_CH[ch] );

    for( ; i<10; i++ )
        if( I2Cwrite( i2c, MUX_ANALOG_ADDR[muxID], data) >= 0 )
            return 0;

    return -1;

}


int config( struct mpsse_context *i2c, int gndMuxCH, int analogMuxId, int analogMuxCh ) {

    if( select_MUX_GND_channel( i2c, gndMuxCH ) < 0 ) {
        printf("ERROR configuring the GND mux\n");
        return -1;
    }

    if( select_MUX_ANALOG_channel( i2c, analogMuxId, analogMuxCh ) < 0 ) {
        printf("ERROR configuring the ANALOG mux\n");
        return -1;
    }

}
