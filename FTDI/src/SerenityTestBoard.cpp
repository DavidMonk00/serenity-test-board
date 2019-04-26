#include "SerenityTestBoard.hpp"

SerenityTestBoard::SerenityTestBoard(void) {
    ftdi = new FT2232H(0x0403, 0x6010, ONE_HUNDRED_KHZ, IFACE_A);
}

SerenityTestBoard::~SerenityTestBoard(void) {
    delete ftdi;
}

float SerenityTestBoard::readADC(void) {
    std::vector<uint8_t> data;
    data.resize(2);
    ftdi->send(ADC_ADDR|I2C_RD, {}, data);
    uint32_t result = ((uint32_t)data[0] & 0xff) << 8;
    result = result | ((uint32_t)data[1] & 0xff);
    return ((float)result) / powf(2,16);
}

Reading SerenityTestBoard::readChannel(int nPoints, int imux, int ich) {
    Reading reading = Reading();
    config(ftdi, GND_MUX[imux][ich], imux, ich );
    // Add sleep to allow voltage to settle to value
    usleep(50e3);
    for (int i = 0; i < nPoints; i++){
        float value = readADC() * ADC_REF_V;
        if (imux == 1 && ich == 4) {
            value = value * 10.0/7.0;
        } else if (imux == 3 && ich == 1) {
            value = value * 24/7.0;
        }
        value += ADC_OFFSET;
        reading.appendData(value);
        usleep(20);
    }
    return reading;
}

std::unordered_map<std::string, Reading> SerenityTestBoard::loopOverChannels(std::vector<std::pair<int, int> > channel_list, int nPoints) {
    #ifdef DEBUG
    printf("All voltages on Serenity (in Volt):\n");
    #endif
    int imux, ich;
    for (auto i : channel_list) {
        imux = i.first;
        ich = i.second;
        readings[MUX_LABLES[imux][ich]] = readChannel(nPoints, imux, ich);
        #ifdef DEBUG
        printf("%s\t", MUX_DISPLAY_LABLES[imux][ich] );
        printf("MUX %d \t CH %d \t ADC_RD %f ( %f )\n",
               imux, ich, readings[MUX_LABLES[imux][ich]].getMean(),
               readings[MUX_LABLES[imux][ich]].getStd());
        #endif
    }
    return readings;
}

void SerenityTestBoard::singleReading(char* mux_label, int nPoints) {
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 0; ich < 8; ich++) {
            if (strcmp(mux_label, MUX_LABLES[imux][ich]) == 0) {
                #ifdef DEBUG
                printf("Selecting MUX %d CHANNEL %d LABEL %s...\n", imux, ich, mux_label);
                #endif
                Reading reading = readChannel(nPoints, imux, ich);
                #ifdef DEBUG
                printf("READING: mean = %f | rms = %f\n", reading.getMean(), reading.getStd());
                #endif
                break;
            }
        }
    }
}

void SerenityTestBoard::writeToFile(std::string reading_type) {
    std::string path = "/home/dmonk/serenity-test-board/FTDI/data/readings/";
    time_t rawtime;
    struct tm *info;
    char buff[64];
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(buff,64,"%Y%m%d%H%M%S", info);
    std::ostringstream os;
    os << path << reading_type << "_" << buff << ".json";
    std::ofstream file;
    file.open(os.str());
    file << "{";
    for (auto i : readings) {
        file << "\"" << i.first << "\": [";
        std::vector<float> data = i.second.getData();
        for (int j = 0; j < data.size(); j++) {
            file << data[j];
            if (j != data.size() - 1) file << ",";
        }
        file << "],";
    }
    file << "}";
    file.close();
}

void SerenityTestBoard::selectMuxChannel(int imux, int ich) {
    #ifdef DEBUG
    printf("Selecting MUX %d CHANNEL %d LABEL %s...\n", imux, ich, MUX_LABLES[imux][ich]);
    #endif
    config(ftdi, GND_MUX[imux][ich], imux, ich );
}

void SerenityTestBoard::selectMuxChannel(char *mux_label) {
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 0; ich < 8; ich++) {
            if (strcmp(mux_label, MUX_LABLES[imux][ich]) == 0) {
                selectMuxChannel(imux, ich);
                break;
            }
        }
    }
}
