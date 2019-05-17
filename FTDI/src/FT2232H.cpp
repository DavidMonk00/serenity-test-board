#include "FT2232H.hpp"

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

FT2232H::FT2232H (int vid, int pid, int freq, int interface) {
    int status = 0;
    if(ftdi_init(&mFtdi) != 0) {
        throw std::runtime_error("Error: FT2232H not initialised");
    }
    ftdi_set_interface(&mFtdi, (ftdi_interface)interface);
    if(ftdi_usb_open_desc_index(&mFtdi, vid, pid, NULL, NULL, 0) != 0) {
        throw std::runtime_error("Error: could not open usb port.");
    }
	mStatus = STOPPED;
    mXsize = I2C_TRANSFER_SIZE;
    status |= ftdi_usb_reset(&mFtdi);
	status |= ftdi_set_latency_timer(&mFtdi, LATENCY_MS);
	status |= ftdi_write_data_set_chunksize(&mFtdi, CHUNK_SIZE);
	status |= ftdi_read_data_set_chunksize(&mFtdi, CHUNK_SIZE);
    status |= ftdi_set_bitmode(&mFtdi, 0, BITMODE_RESET);
    if(status != 0) {
        throw std::runtime_error("Error: ftdi configuration failed.");
    }
    mFtdi.usb_read_timeout = USB_TIMEOUT;
    mFtdi.usb_write_timeout = USB_TIMEOUT;
    ftdi_set_bitmode(&mFtdi, 0, BITMODE_MPSSE);
    setClock(freq);
    setMode();
    mOpen = 1;
    usleep(SETUP_DELAY);
    ftdi_usb_purge_buffers(&mFtdi);
}

FT2232H::~FT2232H () {
    if(mOpen) {
        ftdi_set_bitmode(&mFtdi, 0, BITMODE_RESET);
        ftdi_usb_close(&mFtdi);
        ftdi_deinit(&mFtdi);
    }
}

void FT2232H::read(uint8_t addr, std::vector<uint8_t>& read_data) {
    if (read_data.size() == 0) {
        return;
    }
    start();
    writeBytes((char*) &addr, 1);
    if (getAck() != ACK) {
        throw std::runtime_error("ACK not received after writing address.");
    }
    for (unsigned int i = 0; i < read_data.size(); i++) {
        (i < read_data.size() - 1) ? setAck(ACK) : setAck(NACK);
        read_data[i] = (uint8_t)(*(readByte()));
    }
    stop();
}

void FT2232H::write(uint8_t addr, const std::vector<uint8_t>& write_data) {
    if (write_data.size() == 0) {
        return;
    }
    start();
    writeBytes((char*) &addr, 1);
    if (getAck() != ACK) {
        #ifdef DEBUG
        std::cout << "Address to be written: " << addr << '\n';
        #endif
        throw std::runtime_error("ACK not received after writing address.");
    }
    for (unsigned int i = 0; i < write_data.size(); i++) {
        writeBytes((char*)&(write_data[i]), 1);
        if (getAck() != ACK) {
            throw std::runtime_error("ACK not received after writing data.");
        }
    }
    stop();
}

void FT2232H::send(uint8_t addr, const std::vector<uint8_t>& write_data, std::vector<uint8_t>& read_data) {
    write(addr, write_data);
    read(addr, read_data);
}

void FT2232H::setAck(int ack) {
    if (!mOpen) {
        throw std::runtime_error("Error setting ACK: context not open.");
    }
    if(ack == NACK) {
        mTack = 0xFF;
    } else if (ack == ACK) {
        mTack = 0x00;
    } else {
        throw std::runtime_error("Error setting ack: argument was neither ACK or NACK.");
    }
}

int FT2232H::getAck() {
    return (mOpen) ? (mRack & 0x01) : 0;
}

void FT2232H::start() {
	if (!mOpen) {
        mStatus = STOPPED;
        throw std::runtime_error("Error starting I2C: context not valid.");
    }
	if(mStatus == STARTED) {
		/* Set the default pin states while the clock is low since this is an I2C repeated start condition */
		setBitsLow(mPidle & ~SK);
		/* Make sure the pins are in their default idle state */
		setBitsLow(mPidle);
	}
	/* Set the start condition */
	setBitsLow(mPstart);
	mStatus = STARTED;
}

void FT2232H::rawWrite(unsigned char *buf, int size) {
    if(ftdi_write_data(&mFtdi, buf, size) != size) {
        throw std::runtime_error("Error writing data (raw).");
    }
}

void FT2232H::setBitsLow(int port) {
	char buf[CMD_SIZE] = {(char)SET_BITS_LOW, (char)port, (char)DEFAULT_TRIS};
	return rawWrite((unsigned char *) &buf, sizeof(buf));
}

void FT2232H::stop() {
	if (!mOpen) {
        mStatus = STOPPED;
        throw std::runtime_error("Error stopping I2C: context not open.");
	}
	/* In I2C mode, we need to ensure that the data line goes low while the clock line is low to avoid sending an inadvertent start condition */
	setBitsLow(mPidle & ~DO & ~SK);
	/* Send the stop condition */
	setBitsLow(mPstop);
	/* Restore the pins to their idle states */
	setBitsLow(mPidle);
	mStatus = STOPPED;
}

void FT2232H::setClock(uint32_t freq) {
    enum mpsse_commands {
		TCK_X5			= 0x8A,
		TCK_D5			= 0x8B,
	};
	uint32_t system_clock = 0;
	uint16_t divisor = 0;
	unsigned char buf[CMD_SIZE] = { 0 };
    buf[0] = (freq > SIX_MHZ) ? TCK_X5 : TCK_D5;
    system_clock = (freq > SIX_MHZ) ? SIXTY_MHZ : TWELVE_MHZ;
	rawWrite(buf, 1);
    divisor = (freq <= 0) ? 0xFFFF : freqToDiv(system_clock, freq);
	buf[0] = TCK_DIVISOR;
	buf[1] = (divisor & 0xFF);
	buf[2] = ((divisor >> 8) & 0xFF);
	rawWrite(buf, 3);
}

uint16_t FT2232H::freqToDiv(uint32_t system_clock, uint32_t freq) {
	return (((system_clock / freq) / 2) - 1);
}

uint32_t FT2232H::divToFreq(uint32_t system_clock, uint16_t div) {
	return (system_clock / ((1 + div) * 2));
}

void FT2232H::setMode() {
    enum mpsse_commands {
		DISABLE_ADAPTIVE_CLOCK  = 0x97,
		ENABLE_3_PHASE_CLOCK	= 0x8C,
	};

	int i = 0, setup_commands_size = 0;
	unsigned char buf[CMD_SIZE] = { 0 };
	unsigned char setup_commands[CMD_SIZE*MAX_SETUP_COMMANDS] = { 0 };
		/* Read and write commands need to include endianess */
	mTx   = MPSSE_DO_WRITE | endianess;
	mRx   = MPSSE_DO_READ  | endianess;
	mTxRx = MPSSE_DO_WRITE | MPSSE_DO_READ | endianess;
	/* Clock and chip select pins idle high; all others are low */
	mPidle = mPstart = mPstop = DEFAULT_PORT;
	/* During reads and writes the chip select pin is brought low */
	mPstart &= ~CS;
	/* Disable FT2232H internal loopback */
    unsigned char loopback_buf[1] = { 0 };
	if (mOpen) {
        loopback_buf[0] = (0) ? LOOPBACK_START : LOOPBACK_END;
	    rawWrite(loopback_buf, 1);
	}
	/* Send ACKs by default */
    try {
        setAck(ACK);
    } catch (const std::runtime_error& error) {}
	/* Ensure adaptive clock is disabled */
	setup_commands[setup_commands_size++] = DISABLE_ADAPTIVE_CLOCK;
	/* I2C propogates data on the falling clock edge and reads data on the falling (or rising) clock edge */
	mTx |= MPSSE_WRITE_NEG;
	mRx &= ~MPSSE_READ_NEG;
    /* In I2C, both the clock and the data lines idle high */
	mPidle |= DO | DI;
	/* I2C start bit == data line goes from high to low while clock line is high */
	mPstart &= ~DO & ~DI;
	/* I2C stop bit == data line goes from low to high while clock line is high - set data line low here, so the transition to the idle state triggers the stop condition. */
	mPstop &= ~DO & ~DI;
	/* Enable three phase clock to ensure that I2C data is available on both the rising and falling clock edges */
	setup_commands[setup_commands_size++] = ENABLE_3_PHASE_CLOCK;
	/* Send any setup commands to the chip */
	if(setup_commands_size > 0) {
        rawWrite(setup_commands, setup_commands_size);
	}
	/* Set the idle pin states */
	setBitsLow(mPidle);
    buf[i++] = SET_BITS_HIGH;
    buf[i++] = 0x00;
    buf[i++] = 0xFF;
    rawWrite(buf, i);
}

void FT2232H::writeBytes(char *data, int size) {
	unsigned char *buf = NULL;
	int buf_size = 0, txsize = 1, n = 0;
	if (!mOpen) {
        throw std::runtime_error("Error writing bytes: context not open.");
    }
	while (n < size) {
		buf = buildBlockBuffer(mTx, (unsigned char *) (data + n), txsize, &buf_size);
		if (!buf) {
            throw std::runtime_error("Error: buffer not initialied.");
        }
		rawWrite(buf, buf_size);
		n += txsize;
		free(buf);
		/* Read in the ACK bit and store it in mRack */
		rawRead((unsigned char *) &mRack, 1);
	}
}

unsigned char *FT2232H::buildBlockBuffer(uint8_t cmd, unsigned char *data, int size, int *buf_size) {
	unsigned char *buf = NULL;
   	int i = 0, j = 0, k = 0, dsize = 0, num_blocks = 0, total_size = 0, xfer_size = 0;
 	uint16_t rsize = 0;
	*buf_size = 0;
	/* Data block size is 1 in I2C, or when in bitmode */
	xfer_size = 1;
	num_blocks = (size / xfer_size);
	if(size % xfer_size) {
		num_blocks++;
	}
	/* The total size of the data will be the data size + the write command */
    total_size = size + (CMD_SIZE * num_blocks);
	/* In I2C we have to add 3 additional commands per data block */
	total_size += (CMD_SIZE * 3 * num_blocks);
    buf = (unsigned char*)malloc(total_size);
    if(!buf) {
        throw std::runtime_error("Error: buffer not initialied.");
    }
    memset(buf, 0, total_size);
	for(j = 0; j < num_blocks; j++) {
		dsize = size - k;
		if(dsize > xfer_size) {
            dsize = xfer_size;
		}
		/* The reported size of this block is block size - 1 */
		rsize = dsize - 1;
		/* For I2C we need to ensure that the clock pin is set low prior to clocking out data */
		buf[i++] = SET_BITS_LOW;
		buf[i++] = mPstart & ~SK;
		/* On receive, we need to ensure that the data out line is set as an input to avoid contention on the bus */
		if(cmd == mRx) {
			buf[i++] = DEFAULT_TRIS & ~DO;
		} else {
			buf[i++] = DEFAULT_TRIS;
		}
		/* Copy in the command for this block */
		buf[i++] = cmd;
		buf[i++] = (rsize & 0xFF);
		if(!(cmd & MPSSE_BITMODE)) {
			buf[i++] = ((rsize >> 8) & 0xFF);
		}
		/* On a write, copy the data to transmit after the command */
		if(cmd == mTx || cmd == mTxRx) {
			memcpy(buf+i, data+k, dsize);
			/* i == offset into buf */
			i += dsize;
			/* k == offset into data */
			k += dsize;
		}
		/* In I2C mode we need to clock one ACK bit after each byte */
		/* If we are receiving data, then we need to clock out an ACK for each byte */
		if(cmd == mRx) {
			buf[i++] = SET_BITS_LOW;
			buf[i++] = mPstart & ~SK;
			buf[i++] = DEFAULT_TRIS;
			buf[i++] = mTx | MPSSE_BITMODE;
			buf[i++] = 0;
			buf[i++] = mTack;
		}
		/* If we are sending data, then we need to clock in an ACK for each byte */
		else if(cmd == mTx) {
			/* Need to make data out an input to avoid contention on the bus when the slave sends an ACK */
			buf[i++] = SET_BITS_LOW;
			buf[i++] = mPstart & ~SK;
			buf[i++] = DEFAULT_TRIS & ~DO;

			buf[i++] = mRx | MPSSE_BITMODE;
			buf[i++] = 0;
			buf[i++] = SEND_IMMEDIATE;
		}
	}
	*buf_size = i;
	return buf;
}

void FT2232H::rawRead(unsigned char *buf, int size) {
	int n = 0, r = 0;
	while(n < size) {
		r = ftdi_read_data(&mFtdi, buf, size);
		if(r < 0) break;
		n += r;
	}
}

char *FT2232H::readByte(void) {
	unsigned char *data = NULL, *buf = NULL;
	unsigned char sbuf[SPI_RW_SIZE] = { 0 };
	int rxsize = 0, data_size = 0;
	if (!mOpen) {
        throw std::runtime_error("Error reading byte: context not open.");
    }
	buf = (unsigned char*)malloc(sizeof(char));
	if (!buf) {
        throw std::runtime_error("Error: buffer not initialied.");
    }
	memset(buf, 0, sizeof(char));
    rxsize = (sizeof(char) > mXsize) ? mXsize : sizeof(char);
	data = buildBlockBuffer(mRx, sbuf, rxsize, &data_size);
	if (!data) {
        throw std::runtime_error("Error: buffer not initialied.");
    }
	rawWrite(data, data_size);
	free(data);
	rawRead(buf, rxsize);
	return (char *) buf;
}
