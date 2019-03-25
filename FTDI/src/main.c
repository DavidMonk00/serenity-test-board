/*

   !!! YOU MUST RUN AS ROOT !!!

 */

#include "main.h"

int main(int argc, char** argv) {

    /* */
    int readFlag  = 0;
    int readADCflag  = 0;
    int writeFlag = 0;
    int loopFlag = 0;
    int transmitFlag = 0;
    int singleReadFlag = 0;

    char data = 0x00;
    int  ndata = 1; // numer of bites to read
    char addr = 0x00;
    int port = 1025; // port for external communication
    int npoints = 10;
    char* label = "Not Used";


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
        {"single"        , required_argument,  0, 's'},
        {0,0,0,0}
    };

    int optIndex = 0;
    while ( (opt = getopt_long (argc, argv, "hrwa:d:n:clt:N:s:", longOptions, &optIndex) ) != -1 ) {

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
            printf( "s(--single) : \t read mux og given label.\n" );
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
        case 's':
            singleReadFlag = 1;
            label = optarg;
            break;
        default:
            return 0;
        }
    }

    /* I2C interface */
    struct mpsse_context *i2c = NULL;
    //i2c = MPSSE( I2C, ONE_HUNDRED_KHZ, MSB );
    i2c = OpenIndex(0x0403, 0x6010, I2C, ONE_HUNDRED_KHZ, MSB,
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
                    printf("0x%dx\n", rData[idata]);
                }
                free(rData);
            }
            else {
                printf("Writing operation failed. Error:%d\n", ret);
            }

        }

        /* read ADC */
        if( readADCflag == 1 ) {
            int i;
            for (i = 0; i < npoints; i++) {
                float adcRes = readADC( i2c );
                printf("ADC reading: %f V\n", adcRes);
            }

        }

        if (singleReadFlag == 1) {
            singleReading(i2c, label, npoints);
        }

        /* loop */
        if( loopFlag == 1 ) {
            printf("All voltages on Serenity (in Volt):\n");
            char buffer[1000000];
            loopOverChannels(i2c, npoints, buffer);
            // printf("%s\n", buffer);
            writeToFile(buffer);
            return 0;

        }

        Close(i2c);
        printf( "*** FTDI I2C CONNECTION CLOSED ***\n" );
        return 0 ;

    }

    printf("ERROR >>> Failed to initialize MPSSE: %s\n", ErrorString(i2c));
    printf("fail\n");

    return 0;

}
