#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <vector>

// #include "I2CIO.hpp"
#include "FTDI.hpp"
#include "consts.hpp"
#include "component.hpp"
#include "configuration.hpp"


class FT2232H : Component {
private:
    char buffer[100000];

    struct channel_reading readChannel(int nPoints, int imux, int ich);

public:
    int i2c_enable;
    FTDI* i2c;

    FT2232H (void);
    virtual ~FT2232H (void);

    float readADC(void);
    int loopOverChannels(int nPoints);
    int singleReading(char* mux_label, int nPoints);
    int writeToFile(void);
    void selectMuxChannel(int imux, int ich);
    void selectMuxChannel(char *mux_label);
};
