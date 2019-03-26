#include "configuration.hpp"

int selectI2Cline(I2CIO* i2c, uint32_t line) {
    std::vector<uint8_t> NULL_READ_VECTOR;
    int i = 0;
    for( ; i<10; i++ )
        if(i2c->send(I2C_MUX_ADDR|I2C_WR, {(uint8_t)line}, NULL_READ_VECTOR) >= 0 )
            return 0;
    return -1;
}


int select_MUX_GND_channel(I2CIO* i2c, uint32_t ch){
    std::vector<uint8_t> NULL_READ_VECTOR;
    if( ch<0 || ch>7 ) {
        printf("Ground MUX channel must be in the range [0,7] ");
        return -1;
    }
    selectI2Cline(i2c, MUX_GND_I2C_LINE);
    uint32_t data = ( MUX_EN | MUX_CH[ch] );
    int i = 0;
    for( ; i<10; i++ )
        if(i2c->send(MUX_GND_ADDR|I2C_WR, {(uint8_t)data}, NULL_READ_VECTOR) >= 0)
            return 0;
    return -1;
}


int select_MUX_ANALOG_channel(I2CIO* i2c, uint32_t muxID, uint32_t ch ){
    std::vector<uint8_t> NULL_READ_VECTOR;
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
            if(i2c->send(MUX_ANALOG_ADDR[imux]|I2C_WR, {0}, NULL_READ_VECTOR) >= 0)
                break;
    uint32_t data = ( MUX_EN | MUX_CH[ch] );
    for( ; i<10; i++ )
        if(i2c->send(MUX_ANALOG_ADDR[muxID]|I2C_WR, {(uint8_t)data}, NULL_READ_VECTOR) >= 0)
            return 0;
    return -1;
}


int config(I2CIO* i2c, int gndMuxCH, int analogMuxId, int analogMuxCh ) {
    if( select_MUX_GND_channel(i2c, gndMuxCH) < 0) {
        printf("ERROR configuring the GND mux\n");
        return -1;
    }
    if( select_MUX_ANALOG_channel( i2c, analogMuxId, analogMuxCh ) < 0 ) {
        printf("ERROR configuring the ANALOG mux\n");
        return -1;
    }
}
