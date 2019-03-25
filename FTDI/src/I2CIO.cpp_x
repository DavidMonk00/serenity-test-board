#include "I2CIO.hpp"

/* serenity I2C */
int I2Cwrite( struct mpsse_context *i2c, uint32_t addr, uint32_t data) {

    // printf(" Wring to addr 0x%x: 0x%x\n", addr&0xff, data);
    Start( i2c );                      // start bit

    char wAddr = (addr|I2C_WR);
    Write( i2c, &wAddr, 1 );            // write address and

    if( GetAck(i2c) == ACK ) {
        char d = data;
        Write( i2c, &d, 1 );

        if( GetAck( i2c ) == ACK ) {
            // printf("Writing done.\n");
        } else {
            printf("ERROR >>> I2Cwrite: ACK not received.\n");
            Stop( i2c );
            return -2;
        }
    }
    else {
        printf("ERROR >>> I2Cwrite: ACK not received.\n");
        // printf("ERROR ACK not received.\n");
        Stop( i2c );
        return -1;
    }

    Stop( i2c );

    return 0;

}


int I2Cread( struct mpsse_context *i2c, uint32_t addr, uint32_t *data, uint32_t ndata ) {

    char* rData = NULL;
    int ret = Start( i2c );
    char rAddr = (addr|I2C_RD);
    Write( i2c, &rAddr, 1 );
    if( GetAck(i2c) == ACK ) {
        int idata=0;
        for(; idata<ndata; idata++) {

            if( idata == (ndata-1) ) // send Nack if is the lat read
                SendNacks( i2c );

            rData = Read( i2c, 1 );
            data[idata] = *rData;
                SendAcks( i2c );
        }

    }
    else{
        printf("ERROR >>> I2Cread: ACK not received.\n");
        Stop(i2c);
        return -1;
    }

    Stop(i2c);

  return 0;

}
