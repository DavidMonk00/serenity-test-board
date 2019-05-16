#pragma once

#include <stdint.h>
#include <vector>
#include <string>

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
extern const float ADC_OFFSET;

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

const std::vector<std::pair<int, int> > SERVICES_CHANNELS = {
    {1, 0},
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {2, 5},
    {2, 6},
    {2, 7},
    {3, 1}
};

const std::vector<std::pair<int, int> > X0_CHANNELS = {
    {2, 0},
    {2, 1},
    {2, 2},
    {2, 3},
    {2, 4},
    {3, 2},
    {3, 3},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7}
};

const std::vector<std::pair<int, int> > X1_CHANNELS = {
    {0, 0},
    {0, 1},
    {0, 2},
    {0, 3},
    {0, 4},
    {0, 5},
    {0, 6},
    {0, 7},
    {1, 5},
    {1, 6},
    {1, 7}
};

extern const int GND_MUX[4][8];

struct channel_reading {
  const char* label;
  float* readings;
  float mean;
  float rms;
};
