#include "FT2232H.hpp"

FT2232H::FT2232H(void) {
    i2c = new I2C_serial();
    if (i2c->getStatus()) {
        i2c_enable = 1;
    } else {
        i2c_enable = 0;
    }
}

FT2232H::~FT2232H(void) {
    delete i2c;
}

float FT2232H::readADC(void) {
    uint32_t *data = (uint32_t*) malloc(2);
    i2c->read(ADC_ADDR, data, 2 );
    uint32_t result = (data[0] & 0xff) << 8;
    result = result | (data[1] & 0xff);
    return ((float)result) / powf(2,16);
}

struct channel_reading FT2232H::readChannel(int nPoints, int imux, int ich) {
    struct channel_reading data;
    data.readings = (float*)malloc(nPoints*sizeof(float));
    int confRes = config(i2c, GND_MUX[imux][ich], imux, ich );
    if( confRes < 0 ) {
        printf("ERROR: cannot configure mux.\n");
        return data;
    }
    // Add sleep to allow voltage to settle to value
    usleep(50e3);
    int ipoint = 0;
    float ADCmean=0, ADCrms=0;
    while (ipoint < nPoints) {
        float reading = readADC() * ADC_REF_V;
        if (imux == 1 && ich == 4) {
            reading = reading * 10.0/7.0;
        } else if (imux == 3 && ich == 1) {
            reading = reading * 24/7.0;
        }
        reading += ADC_OFFSET;
        data.readings[ipoint] = reading;
        ADCmean += reading;
        ipoint++;
        usleep(20);
    }
    data.mean = ADCmean/nPoints;

    for( ipoint=0 ; ipoint<nPoints ; ipoint++ ) {
        ADCrms  += pow(data.readings[ipoint] - data.mean, 2);
    }
    data.rms = sqrt(ADCrms/nPoints);
    return data;
}

int FT2232H::loopOverChannels(int nPoints) {
    sprintf(buffer, "%d,", nPoints);
    struct channel_reading data[NUMBER_OF_CHANNELS];
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 7; ich >= 0; ich--) {
            data[8*imux + ich] = readChannel(nPoints, imux, ich);
            data[8*imux + ich].label = MUX_LABLES[imux][ich];
            printf("%s\t", MUX_DISPLAY_LABLES[imux][ich] );
            printf("MUX %d \t CH %d \t ADC_RD %f ( %f )\n",
                   imux, ich, data[8*imux + ich].mean,
                   data[8*imux + ich].rms);
        }
    }
    int i;
    for (i = 0; i < NUMBER_OF_CHANNELS; i++) {
      data[i].label = MUX_LABLES[i / 8][i % 8];
      sprintf(buffer, "%s%s,", buffer, data[i].label);
      int j;
      for(j = 0; j < nPoints ; j++) {
          sprintf(buffer, "%s%f,", buffer,
                  data[i].readings[j]);
      }
    }

}

int FT2232H::singleReading(char* mux_label, int nPoints) {
    struct channel_reading data;
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 0; ich < 8; ich++) {
            if (strcmp(mux_label, MUX_LABLES[imux][ich]) == 0) {
                printf("Selecting MUX %d CHANNEL %d LABEL %s...\n", imux, ich, mux_label);
                data = readChannel(nPoints, imux, ich);
                printf("READING: mean = %f | rms = %f\n", data.mean, data.rms);
                break;
            }
        }
    }
}

int FT2232H::writeToFile() {
  time_t rawtime;
  struct tm *info;
  char buff[160];
  time( &rawtime );
  info = localtime( &rawtime );
  strftime(buff,160,"/home/dmonk/serenity-test-board/FTDI/data/%Y%m%d%H%M%S.dat", info);

  FILE *file = fopen(buff, "w");

  int results = fputs(buffer, file);
  if (results == EOF) {
    return results;
  }
  fclose(file);
  return 0;
}
