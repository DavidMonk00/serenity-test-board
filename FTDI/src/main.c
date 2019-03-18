/*

   !!! YOU MUST RUN AS ROOT !!!

 */

#include "main.h"


float readADC( struct mpsse_context *i2c ) {

    uint32_t *data = (uint32_t*) malloc(2);
    I2Cread( i2c, ADC_ADDR, data, 2 );
    uint32_t result = (data[0] & 0xff) << 8;
    result = result | (data[1] & 0xff);

    // printf("%x\n", data[0]&0xff);
    // printf("%x\n", data[1]&0xff);
    // printf("%d\n", result);

    return ( ((float)result) * ADC_REF_V ) / powf(2,16);

}


int loopOverPP( struct mpsse_context *i2c, int nPoints, char *dataBuf ) {

    // gnd mux must be changed accordingly

    int imux=0;
    float *data = (float*)malloc(nPoints);

    sprintf( dataBuf, "%d,", nPoints );
    //printf("%s\n", "Starting loop over muxes...");
    for( ; imux<4; imux++ ) {
        int ich=0;
        for( ; ich<8; ich++ ) {
            int confRes = configure( i2c, GND_MUX[imux][ich], imux, ich );
            if( confRes < 0 ) {
                free(data);
                return -1;
            }

            // Add sleep to llow voltage to settle to value
            usleep(100e3);

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

            printf("%s\t", MUX_LABLES[imux][ich] );
            sprintf( dataBuf, "%s%s,", dataBuf, MUX_LABLES[imux][ich] );
            printf( "MUX %d \t CH %d \t ADC_RD %f ( %f )\n", imux, ich, ADCmean, ADCrms );

            ipoint=0;
            for(; ipoint<nPoints ; ipoint++) {
                data[ipoint];
                sprintf( dataBuf, "%s%f,", dataBuf, data[ipoint] );
            }


        }
    }

    // free(data);

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
    int npoints = 8;


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
        {"data-points"   , required_argument,  0, 'N'},
        {0,0,0,0}
    };

    int optIndex = 0;
    while ( (opt = getopt_long (argc, argv, "hrwa:d:n:clt:N:", longOptions, &optIndex) ) != -1 ) {

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
            printf( "N(--data-points): <npoints> \t number of points to loop through for each reading.\n" );
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
        case 'N':
            npoints = atoi(optarg);
            break;
        default:
            return 0;
        }
    }

    /* I2C interface */
    struct mpsse_context *i2c = NULL;
    //i2c = MPSSE( I2C, ONE_HUNDRED_KHZ, MSB );
    i2c = OpenIndex(0x0403, 0x6010, I2C, FOUR_HUNDRED_KHZ, MSB,
    	            IFACE_A, NULL, NULL, 0);

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
            loopOverPP( i2c, npoints, buffer );
            writeToFile(buffer);

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
