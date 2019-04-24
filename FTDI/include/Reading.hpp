#pragma once

#include <vector>
#include <cmath>

class Reading {
private:
    std::vector<float> data;

public:
    Reading(void);
    ~Reading (void);
    std::vector<float> getData(void);
    void appendData(float datapoint);
    float getMean(void);
    float getStd(void);
};
