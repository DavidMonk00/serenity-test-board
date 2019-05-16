#pragma once

#include <vector>
#include <stdexcept>
#include <ftdi.h>
#include <string.h>


namespace ftdi {
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
		FIFTY_KHZ = 50000,
		ONE_HUNDRED_KHZ  = 100000,
		FOUR_HUNDRED_KHZ = 400000,
		ONE_MHZ 	 = 1000000,
		TWO_MHZ		 = 2000000,
		FIVE_MHZ	 = 5000000,
		SIX_MHZ 	 = 6000000,
		TEN_MHZ		 = 10000000,
		TWELVE_MHZ 	 = 12000000,
		FIFTEEN_MHZ  = 15000000,
		THIRTY_MHZ 	 = 30000000,
		SIXTY_MHZ 	 = 60000000
	};

	const int MSB = 0x00;
	const int LSB = 0x08;
	const int CHUNK_SIZE = 65535;
	const int SPI_RW_SIZE = (63 * 1024);
	const int SPI_TRANSFER_SIZE = 512;
	const int I2C_TRANSFER_SIZE	= 64;
	const int I2C = 5;

	const int LATENCY_MS = 2;
	const int USB_TIMEOUT = 12e4;
	const int SETUP_DELAY = 25e3;

	const int BITMODE_RESET	= 0;
	const int BITMODE_MPSSE	= 2;

	const int CMD_SIZE = 3;
	const int MAX_SETUP_COMMANDS = 10;
	const int SS_TX_COUNT = 3;

	const int LOW = 0;
	const int HIGH = 1;
	const int NUM_GPIOL_PINS = 4;
	const int NUM_GPIO_PINS = 12;

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

	const int DEFAULT_TRIS = (SK | DO | CS | GPIO0 | GPIO1 | GPIO2 | GPIO3);     /* SK/DO/CS and GPIOs are outputs, DI is an input */
	const int DEFAULT_PORT = (SK | CS);

	enum low_bits_status {
		STARTED,
		STOPPED
	};

	enum i2c_ack {
		ACK  = 0,
		NACK = 1
	};
}

class FT2232H {
private:
	struct ftdi_context mFtdi;
	enum ftdi::low_bits_status mStatus;
    static const int mode = ftdi::I2C;
    static const int endianess = ftdi::MSB;
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
