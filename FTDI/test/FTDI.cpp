#include "FTDI.hpp"

FTDI::FTDI (int vid, int pid, int freq, int interface) {
    int status = 0;
    i2c = (struct mpsse_context*)malloc(sizeof(struct mpsse_context));
    if (i2c) {
        memset(i2c, 0, sizeof(struct mpsse_context));
        if(ftdi_init(&i2c->ftdi) == 0) {
            ftdi_set_interface(&i2c->ftdi, (ftdi_interface)interface);
            int usb_set = ftdi_usb_open_desc_index(&i2c->ftdi, vid, pid, NULL, NULL, 0);
            if(usb_set == 0) {
                i2c->mode = mode;
				i2c->vid = vid;
				i2c->pid = pid;
				i2c->status = STOPPED;
                i2c->endianess = endianess;
                i2c->xsize = I2C_TRANSFER_SIZE;
                status |= ftdi_usb_reset(&i2c->ftdi);
				status |= ftdi_set_latency_timer(&i2c->ftdi, LATENCY_MS);
				status |= ftdi_write_data_set_chunksize(&i2c->ftdi, CHUNK_SIZE);
				status |= ftdi_read_data_set_chunksize(&i2c->ftdi, CHUNK_SIZE);
                status |= ftdi_set_bitmode(&i2c->ftdi, 0, BITMODE_RESET);
                printf("Initialisation status: %d\n", status);
                if(status == 0) {
                    i2c->ftdi.usb_read_timeout = USB_TIMEOUT;
                    i2c->ftdi.usb_write_timeout = USB_TIMEOUT;
                    ftdi_set_bitmode(&i2c->ftdi, 0, BITMODE_MPSSE);
                    setClock(freq);
                    setMode(endianess);
                    i2c->open = 1;
                    usleep(SETUP_DELAY);
                    ftdi_usb_purge_buffers(&i2c->ftdi);
                } else {
                    throw std::runtime_error("Error initialising FTDI.");
                }
            } else {
                printf("Error code: %d\n", usb_set);
                throw std::runtime_error("Error: ftdi unable to open USB.");
            }
        } else {
            throw std::runtime_error("Error: ftdi_init failed.");
        }
    }
}

FTDI::~FTDI () {
    if(i2c) {
        if(i2c->open) {
            ftdi_set_bitmode(&i2c->ftdi, 0, BITMODE_RESET);
            ftdi_usb_close(&i2c->ftdi);
            ftdi_deinit(&i2c->ftdi);
        }
        free(i2c);
        i2c = NULL;
    }
}

void FTDI::read(uint8_t addr, std::vector<uint8_t>& read_data) {
    if (read_data.size() == 0) {
        return;
    }
    start();
    writeBytes((char*) &addr, 1);
    if (getAck() != ACK) {
        throw std::runtime_error("ACK not received after writing address.");
    }
    for (unsigned int i = 0; i < read_data.size(); i++) {
        if (i < read_data.size() - 1) {
            setAck(ACK);
        } else {
            setAck(NACK);
        }
        usleep(1e5);
        read_data[i] = (uint8_t)(*(readByte()));
    }
    stop();
}
void FTDI::write(uint8_t addr, std::vector<uint8_t>& write_data) {
    if (write_data.size() == 0) {
        return;
    }
    start();
    writeBytes((char*) &addr, 1);
    if (getAck() != ACK) {
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
void FTDI::send(uint8_t addr, std::vector<uint8_t>& write_data, std::vector<uint8_t>& read_data) {
    write(addr, write_data);
    read(addr, read_data);
}

void FTDI::setAck(int ack) {
    if (i2c != NULL && i2c->open) {
        if(ack == NACK) {
            i2c->tack = 0xFF;
        } else if (ack == ACK) {
            i2c->tack = 0x00;
        } else {
            throw std::runtime_error("Error setting ack: argument was neither ACK or NACK.");
        }
    } else {
        throw std::runtime_error("Error setting ack: context not valid.");
    }
}

int FTDI::getAck() {
	int ack = 0;
	if (i2c != NULL && i2c->open) {
		ack = (i2c->rack & 0x01);
	}
	return ack;
}

void FTDI::start() {
	if (i2c != NULL && i2c->open) {
		if(i2c->status == STARTED) {
			/* Set the default pin states while the clock is low since this is an I2C repeated start condition */
			setBitsLow(i2c->pidle & ~SK);
			/* Make sure the pins are in their default idle state */
			setBitsLow(i2c->pidle);
		}
		/* Set the start condition */
		setBitsLow(i2c->pstart);
		i2c->status = STARTED;
	} else {
		i2c->status = STOPPED;
        printf("I2C->open : %d\n", i2c->open);
        throw std::runtime_error("Error starting I2C: context not valid.");
	}
}

void FTDI::rawWrite(unsigned char *buf, int size) {
    if(i2c->mode) {
        if(ftdi_write_data(&i2c->ftdi, buf, size) == size) {
        } else {
            throw std::runtime_error("Error writing data (raw).");
        }
    }
}

void FTDI::setBitsLow(int port) {
	char buf[CMD_SIZE] = { 0 };
	buf[0] = SET_BITS_LOW;
	buf[1] = port;
	buf[2] = i2c->tris;
	return rawWrite((unsigned char *) &buf, sizeof(buf));
}

void FTDI::stop() {
	if (i2c != NULL && i2c->open) {
		/* In I2C mode, we need to ensure that the data line goes low while the clock line is low to avoid sending an inadvertent start condition */
		setBitsLow(i2c->pidle & ~DO & ~SK);
		/* Send the stop condition */
		setBitsLow(i2c->pstop);
		/* Restore the pins to their idle states */
		setBitsLow(i2c->pidle);
		i2c->status = STOPPED;
	} else {
		i2c->status = STOPPED;
        throw std::runtime_error("Error stopping I2C: context not valid.");
	}
}

void FTDI::setClock(uint32_t freq) {
	uint32_t system_clock = 0;
	uint16_t divisor = 0;
	unsigned char buf[CMD_SIZE] = { 0 };

	/* Do not call is_valid_context() here, as the FTDI chip may not be completely configured when SetClock is called */
	if(i2c) {
		if(freq > SIX_MHZ) {
			buf[0] = TCK_X5;
			system_clock = SIXTY_MHZ;
		}
		else {
			buf[0] = TCK_D5;
			system_clock = TWELVE_MHZ;
		}
		rawWrite(buf, 1);
		if(freq <= 0) {
			divisor = 0xFFFF;
		}
		else {
			divisor = freqToDiv(system_clock, freq);
		}
		buf[0] = TCK_DIVISOR;
		buf[1] = (divisor & 0xFF);
		buf[2] = ((divisor >> 8) & 0xFF);
		rawWrite(buf, 3);
		i2c->clock = divToFreq(system_clock, divisor);
	} else {
        throw std::runtime_error("Error setting clock: context not valid.");
    }
}

uint16_t FTDI::freqToDiv(uint32_t system_clock, uint32_t freq) {
	return (((system_clock / freq) / 2) - 1);
}

uint32_t FTDI::divToFreq(uint32_t system_clock, uint16_t div) {
	return (system_clock / ((1 + div) * 2));
}

void FTDI::setMode(int endianess) {
	int i = 0, setup_commands_size = 0;
	unsigned char buf[CMD_SIZE] = { 0 };
	unsigned char setup_commands[CMD_SIZE*MAX_SETUP_COMMANDS] = { 0 };

	/* Do not call is_valid_context() here, as the FTDI chip may not be completely configured when SetMode is called */
	if (i2c) {
		/* Read and write commands need to include endianess */
		i2c->tx   = MPSSE_DO_WRITE | endianess;
		i2c->rx   = MPSSE_DO_READ  | endianess;
		i2c->txrx = MPSSE_DO_WRITE | MPSSE_DO_READ | endianess;
		/* Clock, data out, chip select pins are outputs; all others are inputs. */
		i2c->tris = DEFAULT_TRIS;
		/* Clock and chip select pins idle high; all others are low */
		i2c->pidle = i2c->pstart = i2c->pstop = DEFAULT_PORT;
		/* During reads and writes the chip select pin is brought low */
		i2c->pstart &= ~CS;
		/* Disable FTDI internal loopback */
	    setLoopback(0);
		/* Send ACKs by default */
        try {
            setAck(ACK);
        } catch (const std::runtime_error& error) {}
		/* Ensure adaptive clock is disabled */
		setup_commands[setup_commands_size++] = DISABLE_ADAPTIVE_CLOCK;
		/* I2C propogates data on the falling clock edge and reads data on the falling (or rising) clock edge */
		i2c->tx |= MPSSE_WRITE_NEG;
		i2c->rx &= ~MPSSE_READ_NEG;
	    /* In I2C, both the clock and the data lines idle high */
		i2c->pidle |= DO | DI;
		/* I2C start bit == data line goes from high to low while clock line is high */
		i2c->pstart &= ~DO & ~DI;
		/* I2C stop bit == data line goes from low to high while clock line is high - set data line low here, so the transition to the idle state triggers the stop condition. */
		i2c->pstop &= ~DO & ~DI;
		/* Enable three phase clock to ensure that I2C data is available on both the rising and falling clock edges */
		setup_commands[setup_commands_size++] = ENABLE_3_PHASE_CLOCK;
		/* Send any setup commands to the chip */
		if(setup_commands_size > 0) {
            rawWrite(setup_commands, setup_commands_size);
		}
		/* Set the idle pin states */
		setBitsLow(i2c->pidle);
		/* All GPIO pins are outputs, set low */
		i2c->trish = 0xFF;
		i2c->gpioh = 0x00;
        buf[i++] = SET_BITS_HIGH;
        buf[i++] = i2c->gpioh;
        buf[i++] = i2c->trish;
        rawWrite(buf, i);
	} else {
        throw std::runtime_error("Error setting mode: context not valid.");
	}
}

void FTDI::writeBytes(char *data, int size) {
	unsigned char *buf = NULL;
	int buf_size = 0, txsize = 0, n = 0;
	if (i2c != NULL && i2c->open) {
		if (i2c->mode) {
			while (n < size) {
				txsize = 1;
				buf = buildBlockBuffer(i2c->tx, (unsigned char *) (data + n), txsize, &buf_size);
				if (buf) {
					rawWrite(buf, buf_size);
					n += txsize;
					free(buf);
					/* Read in the ACK bit and store it in i2c->rack */
					if(i2c->mode == I2C) {
						rawRead((unsigned char *) &i2c->rack, 1);
					}
				}
			}
		}
	}
}

unsigned char *FTDI::buildBlockBuffer(uint8_t cmd, unsigned char *data, int size, int *buf_size) {
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
    if( buf) {
        memset(buf, 0, total_size);
		for(j = 0; j < num_blocks; j++) {
			dsize = size - k;
			if(dsize > xfer_size) {
                dsize = xfer_size;
			}
			/* The reported size of this block is block size - 1 */
			rsize = dsize - 1;
			/* For I2C we need to ensure that the clock pin is set low prior to clocking out data */
			if(i2c->mode == I2C) {
				buf[i++] = SET_BITS_LOW;
				buf[i++] = i2c->pstart & ~SK;
				/* On receive, we need to ensure that the data out line is set as an input to avoid contention on the bus */
				if(cmd == i2c->rx) {
					buf[i++] = i2c->tris & ~DO;
				} else {
					buf[i++] = i2c->tris;
				}
			}
			/* Copy in the command for this block */
			buf[i++] = cmd;
			buf[i++] = (rsize & 0xFF);
			if(!(cmd & MPSSE_BITMODE)) {
				buf[i++] = ((rsize >> 8) & 0xFF);
			}
			/* On a write, copy the data to transmit after the command */
			if(cmd == i2c->tx || cmd == i2c->txrx) {
				memcpy(buf+i, data+k, dsize);
				/* i == offset into buf */
				i += dsize;
				/* k == offset into data */
				k += dsize;
			}
			/* In I2C mode we need to clock one ACK bit after each byte */
			/* If we are receiving data, then we need to clock out an ACK for each byte */
			if(cmd == i2c->rx) {
				buf[i++] = SET_BITS_LOW;
				buf[i++] = i2c->pstart & ~SK;
				buf[i++] = i2c->tris;
				buf[i++] = i2c->tx | MPSSE_BITMODE;
				buf[i++] = 0;
				buf[i++] = i2c->tack;
			}
			/* If we are sending data, then we need to clock in an ACK for each byte */
			else if(cmd == i2c->tx) {
				/* Need to make data out an input to avoid contention on the bus when the slave sends an ACK */
				buf[i++] = SET_BITS_LOW;
				buf[i++] = i2c->pstart & ~SK;
				buf[i++] = i2c->tris & ~DO;

				buf[i++] = i2c->rx | MPSSE_BITMODE;
				buf[i++] = 0;
				buf[i++] = SEND_IMMEDIATE;
			}
		}
		*buf_size = i;
	}
	return buf;
}

void FTDI::rawRead(unsigned char *buf, int size) {
	int n = 0, r = 0;
	if (i2c->mode) {
		while(n < size) {
			r = ftdi_read_data(&i2c->ftdi, buf, size);
			if(r < 0) break;
			n += r;
		}
		if (i2c->flush_after_read) {
			/*
			 * Make sure the buffers are cleared after a read or subsequent reads may fail.
			 *
			 * Is this needed anymore? It slows down repetitive read operations by ~8%.
			 */
			ftdi_usb_purge_rx_buffer(&i2c->ftdi);
		}
	} else {
        throw std::runtime_error("Error reading data: mode not set.");
    }
}

char *FTDI::readByte(void) {
	unsigned char *data = NULL, *buf = NULL;
	unsigned char sbuf[SPI_RW_SIZE] = { 0 };
	int rxsize = 0, data_size = 0;
	if (i2c != NULL && i2c->open) {
		if (i2c->mode) {
			buf = (unsigned char*)malloc(sizeof(char));
			if (buf)	{
				memset(buf, 0, sizeof(char));
				rxsize = sizeof(char);
				if(rxsize > i2c->xsize) {
					rxsize = i2c->xsize;
				}
				data = buildBlockBuffer(i2c->rx, sbuf, rxsize, &data_size);
				if (data) {
					rawWrite(data, data_size);
					free(data);
					rawRead(buf, rxsize);
				}
			}
		}
	}
	return (char *) buf;
}

void FTDI::setLoopback(int enable) {
	unsigned char buf[1] = { 0 };
	if (i2c != NULL && i2c->open) {
		if(enable) {
			buf[0] = LOOPBACK_START;
		} else {
			buf[0] = LOOPBACK_END;
		}
	    rawWrite(buf, 1);
	}
}
