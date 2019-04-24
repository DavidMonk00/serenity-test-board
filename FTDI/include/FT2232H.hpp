#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <ftdi.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define MPSSE_OK		0
#define MPSSE_FAIL		-1

#define MSB			0x00
#define LSB			0x08

#define CHUNK_SIZE		65535
#define SPI_RW_SIZE		(63 * 1024)
#define SPI_TRANSFER_SIZE	512
#define I2C_TRANSFER_SIZE	64
#define I2C 5

#define LATENCY_MS		2
#define TIMEOUT_DIVISOR		1000000
#define USB_TIMEOUT		120000
#define SETUP_DELAY		25000

#define BITMODE_RESET		0
#define BITMODE_MPSSE		2

#define CMD_SIZE		3
#define MAX_SETUP_COMMANDS	10
#define SS_TX_COUNT		3

#define LOW			0
#define HIGH			1
#define NUM_GPIOL_PINS		4
#define NUM_GPIO_PINS		12

/* FT2232H interfaces */
enum interface {
	IFACE_ANY	= INTERFACE_ANY,
	IFACE_A 	= INTERFACE_A,
	IFACE_B		= INTERFACE_B,
	IFACE_C		= INTERFACE_C,
	IFACE_D		= INTERFACE_D
};

/* Common clock rates */
enum clock_rates {
	ONE_HUNDRED_KHZ  = 100000,
	FOUR_HUNDRED_KHZ = 400000,
	ONE_MHZ 	 = 1000000,
	TWO_MHZ		 = 2000000,
	FIVE_MHZ	 = 5000000,
	SIX_MHZ 	 = 6000000,
	TEN_MHZ		 = 10000000,
	TWELVE_MHZ 	 = 12000000,
	FIFTEEN_MHZ      = 15000000,
	THIRTY_MHZ 	 = 30000000,
	SIXTY_MHZ 	 = 60000000
};

enum pins {
	SK	= 1,
	DO	= 2,
	DI	= 4,
	CS	= 8 ,
	GPIO0	= 16,
	GPIO1	= 32,
	GPIO2	= 64,
	GPIO3	= 128
};

enum i2c_ack {
	ACK  = 0,
	NACK = 1
};

#define DEFAULT_TRIS (SK | DO | CS | GPIO0 | GPIO1 | GPIO2 | GPIO3)     /* SK/DO/CS and GPIOs are outputs, DI is an input */
#define DEFAULT_PORT (SK | CS)       				                    /* SK and CS are high, all others low */

enum mpsse_commands {
	INVALID_COMMAND		= 0xAB,
	ENABLE_ADAPTIVE_CLOCK   = 0x96,
	DISABLE_ADAPTIVE_CLOCK  = 0x97,
	ENABLE_3_PHASE_CLOCK	= 0x8C,
	DISABLE_3_PHASE_CLOCK	= 0x8D,
	TCK_X5			= 0x8A,
	TCK_D5			= 0x8B,
	CLOCK_N_CYCLES		= 0x8E,
	CLOCK_N8_CYCLES		= 0x8F,
	PULSE_CLOCK_IO_HIGH	= 0x94,
	PULSE_CLOCK_IO_LOW	= 0x95,
	CLOCK_N8_CYCLES_IO_HIGH	= 0x9C,
	CLOCK_N8_CYCLES_IO_LOW	= 0x9D,
	TRISTATE_IO		= 0x9E,
};

enum low_bits_status {
	STARTED,
	STOPPED
};


class FT2232H {
private:
	struct ftdi_context mFtdi;
	enum low_bits_status mStatus;
    static const int mode = I2C;
    static const int endianess = MSB;
	int mXsize;
	int mOpen;
	uint8_t mPstart;
	uint8_t mPstop;
	uint8_t mPidle;
	uint8_t mTx;
	uint8_t mRx;
	uint8_t mTxRx;
	uint8_t mTack;
	uint8_t mRack;

    void setAck(int ack);
    int getAck();
    void start();
    void stop();
    void rawWrite(unsigned char *buf, int size);
    void setBitsLow(int port);
    void setClock(uint32_t freq);
    uint16_t freqToDiv(uint32_t system_clock, uint32_t freq);
    uint32_t divToFreq(uint32_t system_clock, uint16_t div);
    void setMode();
    void writeBytes(char *data, int size);
    unsigned char *buildBlockBuffer(uint8_t cmd, unsigned char *data, int size, int *buf_size);
    void rawRead(unsigned char *buf, int size);
    char *readByte(void);

public:
    FT2232H (int vid, int pid, int freq, int interface);
    virtual ~FT2232H (void);
    void read(uint8_t addr, std::vector<uint8_t>& read_data);
    void write(uint8_t addr, const std::vector<uint8_t>& write_data);
    void send(uint8_t addr, const std::vector<uint8_t>& write_data, std::vector<uint8_t>& read_data);
};
