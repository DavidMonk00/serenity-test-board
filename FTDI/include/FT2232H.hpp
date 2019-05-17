#pragma once

#include <vector>
#include <stdexcept>
#include <ftdi.h>
#include <string.h>
#include <iostream>
#include <unistd.h>


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
}

class FT2232H {
private:
	enum low_bits_status {
		STARTED,
		STOPPED
	};

	struct ftdi_context mFtdi;
	enum low_bits_status mStatus;
    static const int endianess = 0x00; // MSB = 0x00, LSB = 0x08
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
