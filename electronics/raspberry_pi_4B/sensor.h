#ifndef SENSOR_H
#define SENSOR_H

#include <vector>
#include <iomanip>
#include <cstdint>
#include <iostream>

typedef std::vector<double> measureData;

class Sensor
{
public:
	// Ctor
    Sensor();
    // Dtor
    ~Sensor() {}

    virtual measureData measure() = 0;

    void initMeasurement();
    bool print(const measureData& data, int inteval, int precision);

    static double getCurrentTime();
	static long getCurrentTimeMillis();

	uint8_t printIndex;

protected:
    Sensor(uint8_t elements) : mData(elements + 1, 0.) {}
    measureData mData;
};


#endif //SENSOR_H
