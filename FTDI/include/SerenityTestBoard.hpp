#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "FT2232H.hpp"
#include "consts.hpp"
#include "configuration.hpp"
#include "Reading.hpp"


class SerenityTestBoard {
private:
    std::unordered_map<std::string, Reading> readings;
    Reading readChannel(int nPoints, int imux, int ich);

public:
    FT2232H* ftdi;

    SerenityTestBoard (void);
    virtual ~SerenityTestBoard (void);

    float readADC(void);
    std::unordered_map<std::string, Reading> loopOverChannels(std::vector<std::pair<int, int> > channel_list, int nPoints);
    void singleReading(char* mux_label, int nPoints);
    void writeToFile(std::string reading_type);
    void selectMuxChannel(int imux, int ich);
    void selectMuxChannel(char *mux_label);
};
