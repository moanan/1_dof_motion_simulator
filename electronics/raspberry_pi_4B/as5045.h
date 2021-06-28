#ifndef AS5311_H
#define AS5311_H

#include <chrono>
#include <math.h>
#include <bitset>
#include <bcm2835.h>

#include "sensor.h"


#define DO RPI_BPLUS_GPIO_J8_13 //
#define CSn RPI_BPLUS_GPIO_J8_18 //
#define CLK RPI_BPLUS_GPIO_J8_16 // 

#define dataSize 18

class AS5045 : public Sensor
{
public:
	// Ctor
    AS5045();
    // Dtor
    ~AS5045() {}

    void updateMagneticData();
    void determineDirection();

    // Measure realization, calls hidden internal measuring function
    measureData measure();

    

protected:
	void initEncoder();

	static const uint8_t COUNT_DATA = 1;
	static const uint8_t delayTime = 1;
	static const uint32_t MASK_POS = 262080; // 0b111111111111000000: mask to obtain first 12 digits with position info
	static constexpr double TO_DISPLACEMENT = 5.0 / (double)(1UL << 12);


	// uint8_t inputStream;
    long angle_raw;
	long turns;
	long angle_raw_previous;
    double position_mm;
    double angleOffset;
};

#endif // AS5311_H