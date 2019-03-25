#include "I2C_serial.hpp"

I2C_serial::I2C_serial(void) {
    m_context = OpenIndex(0x0403, 0x6010, I2C, ONE_HUNDRED_KHZ, MSB,
    	            IFACE_A, NULL, NULL, 0);
}

I2C_serial::~I2C_serial(void) {
    Close(m_context);
}

int I2C_serial::getStatus() {
    return m_context->open;
}

int I2C_serial::write(uint32_t addr, uint32_t data) {
    Start(m_context);
    char wAddr = (addr|I2C_WR);
    Write(m_context, &wAddr, 1);
    if(GetAck(m_context) == ACK) {
        char d = data;
        Write( m_context, &d, 1 );
        if( GetAck( m_context ) != ACK ) {
            printf("ERROR >>> I2Cwrite: ACK not received.\n");
            Stop( m_context );
            return -2;
        }
    }
    else {
        printf("ERROR >>> I2Cwrite: ACK not received.\n");
        Stop(m_context);
        return -1;
    }
    Stop(m_context);
    return 0;
}

int I2C_serial::read(uint32_t addr, uint32_t *data, uint32_t ndata ) {
    char* rData = NULL;
    int ret = Start( m_context );
    char rAddr = (addr|I2C_RD);
    Write( m_context, &rAddr, 1 );
    if( GetAck(m_context) == ACK ) {
        int idata=0;
        for(; idata<ndata; idata++) {
            if( idata == (ndata-1) ) // send Nack if is the lat read
                SendNacks( m_context );
            rData = Read( m_context, 1 );
            data[idata] = *rData;
                SendAcks( m_context );
        }
    }
    else{
        printf("ERROR >>> I2Cread: ACK not received.\n");
        Stop(m_context);
        return -1;
    }
    Stop(m_context);
    return 0;
}
