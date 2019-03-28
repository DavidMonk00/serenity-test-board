#include "main.hpp"

float readADC(FTDI* ftdi) {
    uint8_t read = 0x1;
    uint8_t write = 0x0;
    uint8_t ADCaddr = 0x60;
    const int size = 2;
    std::vector<uint8_t> dat;
    dat.resize(size);
    ftdi->read(ADCaddr|read, dat);
    uint16_t reading = (((uint16_t)dat[0]) << 8) | dat[1];
    float voltage = (float)reading / pow(2,16);
    return voltage*4.096;
}

void testADC(FTDI* ftdi, int N) {
    std::vector<float> data;
    for (int i = 0; i < N; i++) {
        data.push_back(readADC(ftdi));
    }
    for (int i = 0; i < N; i++) {
        printf("%0.3f ", data[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }

    float sum = std::accumulate(data.begin(), data.end(), 0.0);
    float mean = sum / data.size();

    std::vector<float> diff(data.size());
    std::transform(data.begin(), data.end(), diff.begin(), [mean](float x) { return x - mean; });
    float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    float stdev = std::sqrt(sq_sum / data.size());

    printf("\n");
    printf("Mean: %0.3f | std: %0.3f\n", mean, stdev);
}

int main(int argc, char const *argv[]) {
    const int N = 100;
    FTDI* ftdi = new FTDI(0x0403, 0x6010, ONE_HUNDRED_KHZ, IFACE_A);
    printf("Initialised.\n");
    testADC(ftdi, N);
    delete ftdi;
    return 0;
}
