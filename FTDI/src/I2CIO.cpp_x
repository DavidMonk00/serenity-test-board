#include "I2CIO.hpp"

I2CIO::I2CIO(void) {
    int vid = 0x0403;
    int pid = 0x6010;
    static const enum modes mode = I2C;
    static const int freq = ONE_HUNDRED_KHZ;
    static const int endianess = MSB;
    static const int interface = IFACE_A;

    // mpsse = OpenIndex(vid, pid, mode, freq, endianess,
    // 	              interface, NULL, NULL, 0);

    int status = 0;
    mpsse = (struct mpsse_context*)malloc(sizeof(struct mpsse_context));
    if (mpsse) {
        memset(mpsse, 0, sizeof(struct mpsse_context));
        if(ftdi_init(&mpsse->ftdi) == 0) {
            ftdi_set_interface(&mpsse->ftdi, (ftdi_interface)IFACE_A);
            if(ftdi_usb_open_desc_index(&mpsse->ftdi, vid, pid, NULL, NULL, 0) == 0) {
                mpsse->mode = mode;
				mpsse->vid = vid;
				mpsse->pid = pid;
				mpsse->status = STOPPED;
                mpsse->endianess = endianess;
                mpsse->xsize = I2C_TRANSFER_SIZE;
                status |= ftdi_usb_reset(&mpsse->ftdi);
				status |= ftdi_set_latency_timer(&mpsse->ftdi, LATENCY_MS);
				status |= ftdi_write_data_set_chunksize(&mpsse->ftdi, CHUNK_SIZE);
				status |= ftdi_read_data_set_chunksize(&mpsse->ftdi, CHUNK_SIZE);
                status |= ftdi_set_bitmode(&mpsse->ftdi, 0, BITMODE_RESET);
                if(status == 0) {
                    mpsse->ftdi.usb_read_timeout = USB_TIMEOUT;
                    mpsse->ftdi.usb_write_timeout = USB_TIMEOUT;
                    ftdi_set_bitmode(&mpsse->ftdi, 0, BITMODE_MPSSE);
                    if(SetClock(mpsse, freq) == MPSSE_OK) {
                        if(SetMode(mpsse, endianess) == MPSSE_OK) {
                            mpsse->open = 1;
                            usleep(SETUP_DELAY);
                            ftdi_usb_purge_buffers(&mpsse->ftdi);
                        }
                    }
                }
            }
        }
    }


    // struct ftdi_context ftdi;
    // ftdi_init(&ftdi);
}


I2CIO::~I2CIO(void) {
    Close(mpsse);
}


int I2CIO::getStatus() {
    return mpsse->open;
}


int I2CIO::write( uint8_t addr, const std::vector<uint8_t>& write_data) {
    Start(mpsse);
    Write(mpsse, (char*) &addr, 1);

    if( GetAck(mpsse) != ACK) {
        Stop(mpsse);
        throw std::runtime_error( "I2CIO::write - No acknowledge received when writing address." );
    }

    // SetAck(mpsse, NACK);

    // for ( std::vector<uint8_t>::const_iterator lIt( write_data.begin() ) ; lIt != write_data.end() ; ++lIt ) {
    //     Write(mpsse, (char*)( &(*lIt) ) , 1);
    // }
    Write( mpsse, (char*)( &(write_data[0]) ) , write_data.size() );

    if( GetAck(mpsse) != ACK) {
        Stop(mpsse);
        throw std::runtime_error( "I2CIO::write - No acknowledge received after writing data.");
    }

    Stop(mpsse);
}


int I2CIO::read( uint8_t addr, std::vector<uint8_t>& read_data) {
    Start(mpsse);
    //std::cout << "I2C protocol started." << '\n';
    Write(mpsse, (char*) &addr, 1);

    if( GetAck(mpsse) != ACK) {
        Stop(mpsse);
        Close(mpsse);
        throw std::runtime_error( "I2CIO::read - No acknowledge received when writing address." );
    }
    char* d = NULL;
    for (int i = 0; i < read_data.size(); i++) {
        if (i == read_data.size() - 1) {
            printf("Sending NACK\n");
            SendNacks(mpsse);
        } else {
            printf("Sending ACK\n");
            SendAcks(mpsse);
        }
        d = Read(mpsse, 1);
        read_data[i] = (uint8_t)*d;
    }

    // // SetAck(mpsse, ACK);
    // // Start(mpsse);
    // SendAcks(mpsse);
    // char* lData = Read( mpsse, read_data.size() );
    // // for (int i = 0; i < read_data.size(); i++) {
    // //     read_data[i] = (uint8_t)lData[i];
    // //     printf("%d\n", (int)lData[i]);
    // // }
    // read_data.assign(read_data.size(), *lData);
    // // for (auto i : read_data) {
    // //     std::cout << (int)i << '\n';
    // // }
    // SendNacks(mpsse);
    // //Read(mpsse, 1);

    // if( GetAck(mpsse) != ACK) {
    //     Stop(mpsse);
    //     throw std::runtime_error( "I2CIO::read - No acknowledge after reading data." );
    // }

    Stop(mpsse);
}

int I2CIO::send(uint8_t addr, const std::vector<uint8_t>& write_data, std::vector<uint8_t>& read_data) {

    write( addr , write_data );
    read( addr , read_data );

}
