
/*
   !!! YOU MUST RUN AS ROOT !!!
 */

/* C */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* mpasse */
#include <mpsse.h>

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

const float ADC_REF_V = 4.096; // adc reference in volt

/* analog MUXs */

const uint32_t MUX_GND_I2C_LINE = 0x01; // on I2C line 0 (same as I2C_MUX_CH0)
const uint32_t MUX_GND_ADDR = 0x90; // on I2C line 0

const uint32_t MUX_ANALOG_I2C_LINE = 0x02; // on I2C line 1 (same as I2C_MUX_CH1)
const uint32_t MUX_ANALOG_ADDR[4] = { 0x90, 0x92, 0x94, 0x96 };

/* common to analog mux */
const uint32_t MUX_EN  = 0x08;
const uint32_t MUX_CH[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };

const char *MUX0_LABLES[8] = { "X1_POWER_MGTVCCAUX_N        ",
                               "X1_POWER_MGTAVTT_S          ",
                               "X1_POWER_MGTAVCC_S          ",
                               "X1_POWER_MGTAVCC_N          ",
                               "X1_POWER_MGTAVTT_N          ",
                               "X1_POWER_VCCAUX             ",
                               "X1_POWER_+1.8V              ",
                               "X1_POWER_+3.3V              "
};

const char *MUX1_LABLES[8] = { "SERVICES+1.5V               ",
                               "SERVICES+1.0V               ",
                               "SERVICES+1.8V               ",
                               "SERVICES+3.3V               ",
                               "5V_SCALED                   ",
                               "X1_POWER_VCCINT             ",
                               "X1_POWER_MGTVCCAUC_S        ",
                               "X1_POWER_+1.8V_ANALOGUE     "
};

const char *MUX2_LABLES[8] = { "X0_POWER_MGTVCCAUC_N        ",
                               "X0_POWER_+1.8V_ANALOGUE     ",
                               "X0_POWER_MGTVCCAUC_S        ",
                               "X0_POWER_VCCINT             ",
                               "X0_POWER_+3.3V              ",
                               "ARTIX_MGTAVTT               ",
                               "ARTIX_MGTAVCC               ",
                               "SERVICES_POWER_STANDBY_+3.3V"
};


const char *MUX3_LABLES[8] = { "Not Used                    ",
                               "+12_SCALED                  ",
                               "X0_POWER_+1.8V              ",
                               "X0_POWER_VCCAUX             ",
                               "X0_POWER_MGTAVTT_N          ",
                               "X0_POWER_MGTAVCC_N          ",
                               "X0_POWER_MGTAVCC_S          ",
                               "X0_POWER_MGTAVTT_S          "
};

/* serenity I2C */


int I2Cwrite( struct mpsse_context *i2c, uint32_t addr, uint32_t data) {

//    printf(" Wring to addr 0x%x: 0x%x\n", addr&0xff, data);
    Start( i2c );                      // start bit

    char wAddr = (addr|I2C_WR);
    Write( i2c, &wAddr, 1 );            // write address and

    if( GetAck(i2c) == ACK ) {
        char d = data;
        Write( i2c, &d, 1 );

        if( GetAck( i2c ) == ACK ) {
//            printf("Writing done.\n");
        } else {
            printf("ERROR >>> I2Cwrite: ACK not received.\n");
            Stop( i2c );
            return -2;
        }
    }
    else {
        printf("ERROR >>> I2Cwrite: ACK not received.\n");
//        printf("ERROR ACK not received.\n");
        Stop( i2c );
        return -1;
    }

    Stop( i2c );

    return 0;

}



int I2Cread( struct mpsse_context *i2c, uint32_t addr, uint32_t *data, uint32_t ndata ) {

    char* rData = NULL;

    Start( i2c );

    char rAddr = (addr|I2C_RD);
    Write( i2c, &rAddr, 1 );            // read address and
    if( GetAck(i2c) == ACK ) {
        int idata=0;
        for(; idata<ndata; idata++) {

            if( idata<(ndata-1) ) // send Nack if is the lat read
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


float readADC( struct mpsse_context *i2c ) {

    uint32_t *data = (uint32_t*) malloc(2);
    I2Cread( i2c, ADC_ADDR, data, 2 );

    uint32_t result = (data[0] & 0xff) << 8;
    result = result | (data[1] & 0xff);

//    printf("%x\n", data[0]&0xff);
//    printf("%x\n", data[1]&0xff);
//    printf("%d\n", result);

    return ( ((float)result) * ADC_REF_V ) / powf(2,16);

}

int selectI2Cline( struct mpsse_context *i2c, uint32_t line ) {

    int i = 0;
    for( ; i<10; i++ )
        if( I2Cwrite( i2c, I2C_MUX_ADDR, line) >= 0 )
            return 0;

    return -1;

}


int select_MUX_GND_channel( struct mpsse_context *i2c, uint32_t ch ){

    if( ch<0 || ch>7 ) {
        printf("Ground MUX channel must be in the range [0,7] ");
        return -1;
    }

    selectI2Cline( i2c, MUX_GND_I2C_LINE );

    uint32_t data = ( MUX_EN | MUX_CH[ch] );

    int i=0;
    for( ; i<10; i++ )
        if( I2Cwrite( i2c, MUX_GND_ADDR, data) >= 0 )
            return 0;

    return -1;

}


int select_MUX_ANALOG_channel( struct mpsse_context *i2c, uint32_t muxID, uint32_t ch ){

    if( ch<0 || ch>7 ) {
        printf("Analog MUX channel must be in the range [0,7] ");
        return -1;
    }

    if( muxID<0 || muxID>3 ) {
        printf("Analog MUX ID must be in the range [0,3] ");
        return -1;
    }

    selectI2Cline( i2c, MUX_ANALOG_I2C_LINE );

    /* disable all outputs */

    int i=0, imux=0;

    for( ; imux<4; imux++ )
        for( ; i<10; i++ )
            if( I2Cwrite( i2c, MUX_ANALOG_ADDR[imux], 0) >= 0 )
                break;

    uint32_t data = ( MUX_EN | MUX_CH[ch] );

    for( ; i<10; i++ )
        if( I2Cwrite( i2c, MUX_ANALOG_ADDR[muxID], data) >= 0 )
            return 0;

    return -1;

}


int config( struct mpsse_context *i2c, int gndMuxCH, int analogMuxId, int analogMuxCh ) {

    if( select_MUX_GND_channel( i2c, gndMuxCH ) < 0 ) {
        printf("ERROR configuring the GND mux\n");
        return -1;
    }

    if( select_MUX_ANALOG_channel( i2c, analogMuxId, analogMuxCh ) < 0 ) {
        printf("ERROR configuring the ANALOG mux\n");
        return -1;
    }

}


int loopOverPP( struct mpsse_context *i2c, int nPoints, char *dataBuf ) {

    // gnd mux must be changed accordingly

    int imux=0;
    float *data = malloc(nPoints);

    sprintf( dataBuf, "%d,", nPoints );

    for( ; imux<4; imux++ ) {

        int ich=0;
        for( ; ich<8; ich++ ) {

            int confRes = config( i2c, 0, imux, ich );
            if( confRes < 0 ) {
                free(data);
                return -1;
            }

            int ipoint=0;
            float ADCmean=0, ADCrms=0;
            for(; ipoint<nPoints ; ipoint++) {
                data[ipoint] = readADC( i2c );
                ADCmean += data[ipoint];
            }
            ADCmean = ADCmean/nPoints;

            for( ipoint=0 ; ipoint<nPoints ; ipoint++ ) {
                ADCrms  += pow( data[ipoint]-ADCmean, 2 );
            }
            ADCrms = sqrt(ADCrms/nPoints);

            if( imux==0 ) {
                printf("%s\t", MUX0_LABLES[ich] );
                sprintf( dataBuf, "%s%s,", dataBuf, MUX0_LABLES[ich] );
            }
            else if( imux==1 ) {
                printf("%s\t", MUX1_LABLES[ich] );
                sprintf( dataBuf, "%s%s,", dataBuf, MUX1_LABLES[ich] );
            }
            else if( imux==2 ) {
                printf("%s\t", MUX2_LABLES[ich] );
                sprintf( dataBuf, "%s%s,", dataBuf, MUX2_LABLES[ich] );
            }
            else if( imux==3 ) {
                printf("%s\t", MUX3_LABLES[ich] );
                sprintf( dataBuf, "%s%s,", dataBuf, MUX3_LABLES[ich] );
            }
            printf( "MUX %d \t CH %d \t ADC_RD %f ( %f )\n", imux, ich, ADCmean, ADCrms );

            ipoint=0;
            for(; ipoint<nPoints ; ipoint++) {
                data[ipoint];
                sprintf( dataBuf, "%s%f,", dataBuf, data[ipoint] );
            }


        }
    }

//    free(data);

    return 0;

}


int main(int argc, char** argv) {

    /* */
    int readFlag  = 0;
    int readADCflag  = 0;
    int writeFlag = 0;
    int loopFlag = 0;
    int transmitFlag = 0;

    char data = 0x00;
    int  ndata = 1; // numer of bites to read
    char addr = 0x00;
    int port = 1025; // port for external communication


    /* options */
    int opt;
    const struct option longOptions[] = {
        {"help"          ,  no_argument     ,  0, 'h'},
        {"read"          , required_argument,  0, 'r'},
        {"write"         , required_argument,  0, 'w'},
        {"addr"          , required_argument,  0, 'a'},
        {"data"          , required_argument,  0, 'd'},
        {"ndata"         , required_argument,  0, 'n'},
        {"adc"           , required_argument,  0, 'c'},
        {"loop"          , required_argument,  0, 'l'},
        {"transmit"      , required_argument,  0, 't'},
        {0,0,0,0}
    };

    int optIndex = 0;
    while ( (opt = getopt_long (argc, argv, "hrwa:d:n:clt:", longOptions, &optIndex) ) != -1 ) {

        switch (opt)
        {
        case 'h':
            printf( "Help.\n" );
            printf( "h(--help  ) : \t shows this help.\n" );
            printf( "r(--read  ) : \t will read from the <addr>.\n" );
            printf( "w(--write ) : \t will write to the <addr> what is in <data>.\n" );
            printf( "a(--addr  ) <addr>: \t addr of the I2C device (hex).\n" );
            printf( "d(--data  ) <data>: \t data to write (hex).\n" );
            printf( "n(--ndata ) <ndata>: \t number of data bytes to read (int).\n" );
            printf( "c(--adc   ) : \t returns the ADC conversion in V.\n" );
            printf( "l(--loop  ) : \t returns all the voltages on each mux.\n" );
            printf( "t(--transmit): <port> \t will continuosly transmit data to <port>.\n" );
            return 0;
            break;
        case 'r':
            readFlag  = 1;
            break;
        case 'w':
            writeFlag = 1;
            break;
        case 'a':
            addr = strtol(optarg, NULL, 16);
            break;
        case 'd':
            data = strtol(optarg, NULL, 16);
            break;
        case 'n':
            ndata = atoi(optarg);
            break;
        case 'c':
            readADCflag = 1;
            break;
        case 'l':
            loopFlag = 1;
            break;
        case 't':
            port = atoi(optarg);
            loopFlag = 1;
            transmitFlag = 1;
            break;
        default:
            return 0;
        }
    }

    /* I2C interface */
    struct mpsse_context *i2c = NULL;
    i2c = MPSSE( I2C, FOUR_HUNDRED_KHZ, MSB );

    if( i2c != NULL && i2c->open ) {

        printf( "*** FTDI I2C CONNECTION OPEN ***\n" );

        /* write to device */
        if( writeFlag==1 ) {
            int ret = I2Cwrite( i2c, addr, data );
            if( ret >= 0 ) {
                printf("Writing operation done.\n");
            }
            else {
                printf("Writing operation failed. Error:%d\n", ret);
            }
        }

        /* read to device */
        if( readFlag==1 ) {
            uint32_t *rData = (uint32_t*) malloc (ndata);
            int ret = I2Cread( i2c, addr, rData, ndata );
            if( ret >= 0 ){
                printf("Reading operation done, date read:\n");
                int idata=0;
                for(; idata<ndata; idata++) {
                    printf("0x%lx\n", rData[idata]);
                }
                free(rData);
            }
            else {
                printf("Writing operation failed. Error:%d\n", ret);
            }

        }

        /* read ADC */
        if( readADCflag == 1 ) {
            float adcRes = readADC( i2c );
            printf("ADC reading: %f V\n", adcRes);
        }

        /* loop */
        if( loopFlag == 1 ) {
            printf("All voltages on Serenity (in Volt):\n");
            char buffer[1000000];
            loopOverPP( i2c, 10, buffer );

            if( transmitFlag==1 ) {
                int sockfd, portno, n;

                struct sockaddr_in serv_addr;
                struct hostent *server;

                char hostname[] = "localhost";

                portno = port;

                sockfd = socket(AF_INET, SOCK_STREAM, 0);

                if (sockfd < 0)
                    error("ERROR opening socket");
                server = gethostbyname( hostname );

                if (server == NULL) {
                    fprintf(stderr,"ERROR, no such host\n");
                    exit(0);
                }

                memset( &serv_addr, 0, sizeof( serv_addr ) );
                serv_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr,
                      (char *)&serv_addr.sin_addr.s_addr,
                      server->h_length);
                serv_addr.sin_port = htons(portno);

                if ( connect( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0) {
                    error("ERROR connecting");

                }

                n = write( sockfd, buffer, strlen(buffer) );
                if ( n < 0 )
                    error("ERROR writing to socket");

                close( sockfd );

            }

            return 0;

        }

        Close(i2c);
        printf( "*** FTDI I2C CONNECTION CLOSE ***\n" );
        return 0 ;

    }

printf("ERROR >>> Failed to initialize MPSSE: %s\n", ErrorString(i2c));
    printf("fail\n");

    return 0;

}
