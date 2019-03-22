#include "FTDI.h"

float readADC( struct mpsse_context *i2c ) {

    uint32_t *data = (uint32_t*) malloc(2);
    I2Cread( i2c, ADC_ADDR, data, 2 );
    uint32_t result = (data[0] & 0xff) << 8;
    result = result | (data[1] & 0xff);

    return (((float)result)) / powf(2,16);

}


struct channel_reading readChannel(struct mpsse_context *i2c, int nPoints, int imux, int ich) {
    struct channel_reading data;
    data.readings = (float*)malloc(nPoints*sizeof(float));
    int confRes = config( i2c, GND_MUX[imux][ich], imux, ich );
    if( confRes < 0 ) {
        printf("ERROR: cannot configure mux.\n");
        return data;
    }
    // Add sleep to allow voltage to settle to value
    usleep(50e3);
    int ipoint = 0;
    float ADCmean=0, ADCrms=0;
    while (ipoint < nPoints) {
        float reading = readADC(i2c) * ADC_REF_V;
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


int loopOverChannels(struct mpsse_context *i2c, int nPoints, char* dataBuf) {
    sprintf( dataBuf, "%d,", nPoints );
    struct channel_reading data[NUMBER_OF_CHANNELS];
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 7; ich >= 0; ich--) {
            data[8*imux + ich] = readChannel(i2c, nPoints, imux, ich);
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
      sprintf(dataBuf, "%s%s,", dataBuf, data[i].label);
      int j;
      for(j = 0; j < nPoints ; j++) {
          sprintf(dataBuf, "%s%f,", dataBuf,
                  data[i].readings[j]);
      }
    }

}


int singleReading(struct mpsse_context *i2c, char* mux_label, int nPoints) {
    struct channel_reading data;
    int imux, ich;
    for(imux = 0; imux < 4; imux++) {
        for(ich = 0; ich < 8; ich++) {
            if (strcmp(mux_label, MUX_LABLES[imux][ich]) == 0) {
                printf("Selecting MUX %d CHANNEL %d LABEL %s...\n", imux, ich, mux_label);
                data = readChannel(i2c, nPoints, imux, ich);
                printf("READING: mean = %f | rms = %f\n", data.mean, data.rms);
                break;
            }
        }
    }
}
