#include "FT2232H.hpp"

FT2232H::FT2232H(void) {
    i2c = OpenIndex(0x0403, 0x6010, I2C, ONE_HUNDRED_KHZ, MSB,
    	            IFACE_A, NULL, NULL, 0);
}

FT2232H::~FT2232H(void) {}

float FT2232H::readADC(void) {
    uint32_t *data = (uint32_t*) malloc(2);
    I2Cread( i2c, ADC_ADDR, data, 2 );
    uint32_t result = (data[0] & 0xff) << 8;
    result = result | (data[1] & 0xff);
    return ((float)result) / powf(2,16);
}
