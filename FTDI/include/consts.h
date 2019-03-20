#pragma once

#include <stdlib.h>
#include <netinet/in.h>

#define NUMBER_OF_CHANNELS 32

extern const uint32_t I2C_WR;
extern const uint32_t I2C_RD;

/* I2C MUX */
extern const uint32_t I2C_MUX_ADDR;

extern const uint32_t I2C_MUX_CH0;
extern const uint32_t I2C_MUX_CH1;
extern const uint32_t I2C_MUX_CH2;
extern const uint32_t I2C_MUX_CH3;
extern const uint32_t I2C_MUX_CH4;
extern const uint32_t I2C_MUX_CH5;
extern const uint32_t I2C_MUX_CH6;
extern const uint32_t I2C_MUX_CH7;

/* ADC */
extern const uint32_t ADC_ADDR;

extern const float ADC_REF_V; // adc reference in volt

/* analog MUXs */

extern const uint32_t MUX_GND_I2C_LINE; // on I2C line 0 (same as I2C_MUX_CH0)
extern const uint32_t MUX_GND_ADDR; // on I2C line 0

extern const uint32_t MUX_ANALOG_I2C_LINE; // on I2C line 1 (same as I2C_MUX_CH1)
extern const uint32_t MUX_ANALOG_ADDR[4];

/* common to analog mux */
extern const uint32_t MUX_EN;
extern const uint32_t MUX_CH[8];

extern const char *MUX_LABLES[4][8];
extern const char *MUX_DISPLAY_LABLES[4][8];

extern const int GND_MUX[4][8];

struct channel_reading {
  const char* label;
  float* readings;
  float mean;
  float rms;
};
