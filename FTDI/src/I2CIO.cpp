#include "I2CIO.hpp"

I2CIO::I2CIO(void) {
    m_context = OpenIndex(0x0403, 0x6010, I2C, ONE_HUNDRED_KHZ, MSB,
    	                  IFACE_A, NULL, NULL, 0);

    // struct ftdi_context ftdi;
    // ftdi_init(&ftdi);
}

I2CIO::~I2CIO(void) {
    Close(m_context);
}

int I2CIO::getStatus() {
    return m_context->open;
}

int I2CIO::send(uint8_t addr, const std::vector<uint8_t>& write_data, std::vector<uint8_t>& read_data) {
    char *rData = NULL;
    Start(m_context);
    char address = (char)addr;
    Write(m_context, &address, 1);
    if(GetAck(m_context) == ACK) {
        if (read_data.size() > 0) {
            for (int i = 0; i < read_data.size(); i++) {
                if(i == (read_data.size() - 1)) {
                    SendNacks(m_context);
                }// send Nack if is the lat read
                rData = Read(m_context, 1);
                read_data[i] = *rData;
                SendAcks(m_context);
            }
        } else if (write_data.size() > 0) {
            for (auto i : write_data) {
                char d = (char)i;
                Write(m_context, &d, 1);
            }
        }
    }
    Stop(m_context);
}
