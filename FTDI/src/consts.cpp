#include "consts.hpp"

const uint32_t I2C_WR = 0x00;
const uint32_t I2C_RD = 0x01;

/* I2C MUX */
const uint32_t I2C_MUX_ADDR = 0xE0;

const uint32_t I2C_MUX_CH0 = 0x01;
const uint32_t I2C_MUX_CH1 = 0x02;
const uint32_t I2C_MUX_CH2 = 0x04;
const uint32_t I2C_MUX_CH3 = 0x08;
const uint32_t I2C_MUX_CH4 = 0x10;
const uint32_t I2C_MUX_CH5 = 0x20;
const uint32_t I2C_MUX_CH6 = 0x40;
const uint32_t I2C_MUX_CH7 = 0x80;

/* ADC */
const uint32_t ADC_ADDR  = 0x60;
const float ADC_REF_V = 4.096;//4.18125;//4.116; // adc reference in volt
const float ADC_OFFSET = 0.20;

/* analog MUXs */
const uint32_t MUX_GND_I2C_LINE = 0x01; // on I2C line 0 (same as I2C_MUX_CH0)
const uint32_t MUX_GND_ADDR = 0x90; // on I2C line 0
const uint32_t MUX_ANALOG_I2C_LINE = 0x02; // on I2C line 1 (same as I2C_MUX_CH1)
const uint32_t MUX_ANALOG_ADDR[4] = { 0x90, 0x92, 0x94, 0x96 };

/* common to analog mux */
const uint32_t MUX_EN  = 0x08;
const uint32_t MUX_CH[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

const char *MUX_LABLES[4][8] = {
  { "X1_POWER_MGTVCCAUX_N",
    "X1_POWER_MGTAVTT_S",
    "X1_POWER_MGTAVCC_S",
    "X1_POWER_MGTAVCC_N",
    "X1_POWER_MGTAVTT_N",
    "X1_POWER_VCCAUX",
    "X1_POWER_+1.8V",
    "X1_POWER_+3.3V"},
  { "SERVICES+1.5V",
    "SERVICES+1.0V",
    "SERVICES+1.8V",
    "SERVICES+3.3V",
    "5V_SCALED",
    "X1_POWER_VCCINT",
    "X1_POWER_MGTVCCAUC_S",
    "X1_POWER_+1.8V_ANALOGUE"},
  { "X0_POWER_MGTVCCAUC_N",
    "X0_POWER_+1.8V_ANALOGUE",
    "X0_POWER_MGTVCCAUC_S",
    "X0_POWER_VCCINT",
    "X0_POWER_+3.3V",
    "ARTIX_MGTAVTT",
    "ARTIX_MGTAVCC",
    "SERVICES_POWER_STANDBY_+3.3V"},
  { "Not Used",
    "+12_SCALED",
    "X0_POWER_+1.8V",
    "X0_POWER_VCCAUX",
    "X0_POWER_MGTAVTT_N",
    "X0_POWER_MGTAVCC_N",
    "X0_POWER_MGTAVCC_S",
    "X0_POWER_MGTAVTT_S"}
  };

  const char *MUX_DISPLAY_LABLES[4][8] = {
    { "X1_POWER_MGTVCCAUX_N        ",
      "X1_POWER_MGTAVTT_S          ",
      "X1_POWER_MGTAVCC_S          ",
      "X1_POWER_MGTAVCC_N          ",
      "X1_POWER_MGTAVTT_N          ",
      "X1_POWER_VCCAUX             ",
      "X1_POWER_+1.8V              ",
      "X1_POWER_+3.3V              "},
    { "SERVICES+1.5V               ",
      "SERVICES+1.0V               ",
      "SERVICES+1.8V               ",
      "SERVICES+3.3V               ",
      "5V_SCALED                   ",
      "X1_POWER_VCCINT             ",
      "X1_POWER_MGTVCCAUC_S        ",
      "X1_POWER_+1.8V_ANALOGUE     "},
    { "X0_POWER_MGTVCCAUC_N        ",
      "X0_POWER_+1.8V_ANALOGUE     ",
      "X0_POWER_MGTVCCAUC_S        ",
      "X0_POWER_VCCINT             ",
      "X0_POWER_+3.3V              ",
      "ARTIX_MGTAVTT               ",
      "ARTIX_MGTAVCC               ",
      "SERVICES_POWER_STANDBY_+3.3V"},
    { "Not Used                    ",
      "+12_SCALED                  ",
      "X0_POWER_+1.8V              ",
      "X0_POWER_VCCAUX             ",
      "X0_POWER_MGTAVTT_N          ",
      "X0_POWER_MGTAVCC_N          ",
      "X0_POWER_MGTAVCC_S          ",
      "X0_POWER_MGTAVTT_S          "}
    };

const int GND_MUX[4][8] = {
  {2, 2, 2, 2, 2, 2, 2, 2},
  {0, 0, 0, 0, 0, 2, 2, 2},
  {1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1}
};
