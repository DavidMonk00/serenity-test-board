#include "configuration.hpp"

void selectI2Cline(FT2232H* ftdi, uint32_t line) {
    std::vector<uint8_t> NULL_READ_VECTOR;
    ftdi->send(I2C_MUX_ADDR|I2C_WR, {(uint8_t)line}, NULL_READ_VECTOR);
}


void select_MUX_GND_channel(FT2232H* ftdi, uint32_t ch){
    std::vector<uint8_t> NULL_READ_VECTOR;
    if( ch<0 || ch>7 ) {
        throw std::runtime_error("Ground MUX channel must be in the range [0,7] ");
    }
    selectI2Cline(ftdi, MUX_GND_I2C_LINE);
    uint32_t data = ( MUX_EN | MUX_CH[ch] );
    ftdi->send(MUX_GND_ADDR|I2C_WR, {(uint8_t)data}, NULL_READ_VECTOR);
}


void select_MUX_ANALOG_channel(FT2232H* ftdi, uint32_t muxID, uint32_t ch ){
    std::vector<uint8_t> NULL_READ_VECTOR;
    if( ch<0 || ch>7 ) {
        throw std::runtime_error("Analog MUX channel must be in the range [0,7] ");
    }
    if( muxID<0 || muxID>3 ) {
        throw std::runtime_error("Analog MUX ID must be in the range [0,3] ");
    }
    selectI2Cline(ftdi, MUX_ANALOG_I2C_LINE);
    /* disable all outputs */
    for(int imux = 0; imux < 4; imux++)
        ftdi->send(MUX_ANALOG_ADDR[imux]|I2C_WR, {0}, NULL_READ_VECTOR);
    uint32_t data = ( MUX_EN | MUX_CH[ch] );
    ftdi->send(MUX_ANALOG_ADDR[muxID]|I2C_WR, {(uint8_t)data}, NULL_READ_VECTOR);
}


void config(FT2232H* ftdi, int gndMuxCH, int analogMuxId, int analogMuxCh ) {
    select_MUX_GND_channel(ftdi, gndMuxCH);
    select_MUX_ANALOG_channel( ftdi, analogMuxId, analogMuxCh );
}
