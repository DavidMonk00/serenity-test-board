#include "Reading.hpp"

Reading::Reading(void) {
}

Reading::~Reading(void) {}

std::vector<float> Reading::getData(void) {
    return data;
}

void Reading::appendData(float datapoint) {
    data.push_back(datapoint);
}

float Reading::getMean(void) {
    float sum = 0;
    for (auto i : data) {
        sum += i;
    }
    return sum / data.size();
}

float Reading::getStd(void) {
    float sum_square = 0;
    float mean = getMean();
    if (mean == 0) getMean();
    for (auto i : data) {
        sum_square += pow(i - mean, 2);
    }
    return sqrt(sum_square /  data.size());
}
