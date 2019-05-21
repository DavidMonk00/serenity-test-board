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
    int sweep_flag = 0;

    char data = 0x00;
    int  ndata = 1; // numer of bytes to read
    char addr = 0x00;
    int port = 1025; // port for external communication
    int npoints = 10;
    char* label = (char*)"Not Used";
    char* category = (char*)"services";


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
        {"list"          , required_argument,  0, 'l'},
        {"transmit"      , required_argument,  0, 't'},
        {"data-points"   , required_argument,  0, 'N'},
        {"single"        , required_argument,  0, 's'},
        {"mux"           , required_argument,  0, 'm'},
        {"channel"       , required_argument,  0, 'x'},
        {"label"         , required_argument,  0, 'L'},
        {"sweep"         , no_argument,        0, 'S'},
        {0,0,0,0}
    };

    int optIndex = 0;
    while ( (opt = getopt_long (argc, argv, "hrwa:d:n:clt:N:s:m:x:L:S", longOptions, &optIndex) ) != -1 ) {

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
            printf( "l(--list  ) : \t returns all the voltages on each mux.\n" );
            printf( "t(--transmit): <port> \t will continuosly transmit data to <port>.\n" );
            printf( "N(--data-points): <npoints> \t number of points to loop through for each reading.\n" );
            printf( "s(--single) <label>: \t read mux of given label.\n" );
            printf( "m(--mux  ) <index>: \t Index of mux to select.\n" );
            printf( "x(--channel  ) <index>: \t Index of channel to select.\n" );
            printf( "L(--label) <label> \t configure muxes to select given label.\n");
            printf( "S(--sweep ) : \t returns all the services voltages.\n" );
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
            category = optarg;
            std::cout << optarg << '\n';
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
        enum category_values {c_all, c_services, c_x0, c_x1};
        std::map<std::string, category_values> s_mapStringValues;
        s_mapStringValues["all"] = c_all;
        s_mapStringValues["services"] = c_services;
        s_mapStringValues["x0"] = c_x0;
        s_mapStringValues["x1"] = c_x1;
        std::string s_category = std::string(category);
        std::vector<std::pair<int, int> > channels;
        switch (s_mapStringValues[s_category]) {
            case c_services:
            channels = SERVICES_CHANNELS;
            break;
            case c_x0:
            channels = X0_CHANNELS;
            break;
            case c_x1:
            channels = X1_CHANNELS;
            break;
            case c_all:
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 8; j++) {
                    channels.push_back({i, j});
                }
            }
            break;
        }
        stb->loopOverChannels(channels, npoints);
        stb->writeToFile(s_category);
    }

    if (sweep_flag) {
        for (uint8_t addr = 0; addr < 0x100; adrr++) {
            std::vector<uint8_t> rData;
            rData.resize(ndata);
            stb->ftdi->send(addr|I2C_RD, {}, rData);
            std::cout << addr << '\n';
            printf("Reading operation done, data read:\n");
            for(auto i : rData) {
                printf("0x%x\n", i);
            }
        }
    }

    if (muxConfigFlag) stb->selectMuxChannel(mux_index, channel_index);

    if (mux_label_flag) stb->selectMuxChannel(label);

    delete stb;
    return 0 ;
}
