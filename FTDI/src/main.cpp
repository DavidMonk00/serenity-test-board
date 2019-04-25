#include "main.hpp"

int main(int argc, char** argv) {
    int readFlag  = 0;
    int readADCflag  = 0;
    int writeFlag = 0;
    int loopFlag = 0;
    int transmitFlag = 0;
    int singleReadFlag = 0;
    int muxConfigFlag = 0;
    int mux_index = 0;
    int channel_index = 0;
    int mux_label_flag = 0;
    int services_flag = 0;

    char data = 0x00;
    int  ndata = 1; // numer of bytes to read
    char addr = 0x00;
    int port = 1025; // port for external communication
    int npoints = 10;
    char* label = (char*)"Not Used";


    /* options */
    int opt;
    const struct option longOptions[] = {
        {"help"          , no_argument,        0, 'h'},
        {"read"          , no_argument,        0, 'r'},
        {"write"         , no_argument,        0, 'w'},
        {"addr"          , required_argument,  0, 'a'},
        {"data"          , required_argument,  0, 'd'},
        {"ndata"         , required_argument,  0, 'n'},
        {"adc"           , no_argument,        0, 'c'},
        {"all"           , no_argument,        0, 'l'},
        {"transmit"      , required_argument,  0, 't'},
        {"data-points"   , required_argument,  0, 'N'},
        {"single"        , required_argument,  0, 's'},
        {"mux"           , required_argument,  0, 'm'},
        {"channel"       , required_argument,  0, 'x'},
        {"label"         , required_argument,  0, 'L'},
        {"services"      , no_argument,        0, 'S'},
        {0,0,0,0}
    };

    int optIndex = 0;
    while ( (opt = getopt_long (argc, argv, "hrwa:d:n:clt:N:s:m:x:L:", longOptions, &optIndex) ) != -1 ) {

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
            printf( "l(--all  ) : \t returns all the voltages on each mux.\n" );
            printf( "t(--transmit): <port> \t will continuosly transmit data to <port>.\n" );
            printf( "N(--data-points): <npoints> \t number of points to loop through for each reading.\n" );
            printf( "s(--single) <label>: \t read mux of given label.\n" );
            printf( "m(--mux  ) <index>: \t Index of mux to select.\n" );
            printf( "x(--channel  ) <index>: \t Index of channel to select.\n" );
            printf( "L(--label) <label> \t configure muxes to select given label.\n");
            printf( "S(--services ) : \t returns all the services voltages.\n" );
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
        case 'm':
            muxConfigFlag = 1;
            mux_index = atoi(optarg);
            break;
        case 'x':
            muxConfigFlag = 1;
            channel_index = atoi(optarg);
            break;
        case 'L':
            mux_label_flag = 1;
            label = optarg;
            break;
        case 'S':
            services_flag = 1;
            break;
        default:
            return 0;
        }
    }

    /* I2C interface */
    SerenityTestBoard* stb = new SerenityTestBoard();
    if( writeFlag==1 ) {
        std::vector<uint8_t> NULL_READ_VECTOR;
        stb->ftdi->send(addr|I2C_WR, {(uint8_t)data}, NULL_READ_VECTOR);
    }

    if(readFlag==1) {
        std::vector<uint8_t> rData;
        rData.resize(ndata);
        stb->ftdi->send(addr|I2C_RD, {}, rData);
        printf("Reading operation done, data read:\n");
        for(auto i : rData) {
            printf("0x%x\n", i);
        }
    }

    if(readADCflag == 1) {
        int i;
        for (i = 0; i < npoints; i++) {
            float adcRes = stb->readADC();
            printf("ADC reading: %f V\n", adcRes);
        }
    }

    if (singleReadFlag == 1) stb->singleReading(label, npoints);

    if(loopFlag == 1) {
        std::vector<std::pair<int, int> > v;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 8; j++) {
                v.push_back({i, j});
            }
        }
        stb->loopOverChannels(v, npoints);
        stb->writeToFile("all");
    }

    if (services_flag) {
        for (auto i : SERVICES_CHANNELS) {
            std::cout << i.first << i.second << '\n';
        }
    }

    if (muxConfigFlag) stb->selectMuxChannel(mux_index, channel_index);

    if (mux_label_flag) stb->selectMuxChannel(label);

    delete stb;
    return 0 ;
}
