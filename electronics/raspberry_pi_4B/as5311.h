#ifndef AS5311_H
#define AS5311_H

#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <bitset>
#include <bcm2835.h>
#include <deque>
#include <algorithm>

#include "sensor.h"
#include "global.h"


#define DO  4 // data pin
#define CLK 3 // clock pin
#define CSn 2 // chip select pin

#define dataSize 18 // Depends on SSI Data Format. There are options for 16/14/12/10 Bits Data + 4 Bits Status {Ready, MHI, MLO, Parity}

class AS5311 : public Sensor
{
public:
	// Ctor
    AS5311();
    // Dtor
    ~AS5311() {}

    void updateMagneticData();
    void determineDirection();

    // Measure realization, calls hidden internal measuring function
    measureData measure();

protected:
	void initEncoder();

	// https://stackoverflow.com/questions/3531060/how-to-initialize-a-static-const-member-in-c/3531105
	static const uint8_t COUNT_DATA = 1;
	static const uint8_t delayTime = 1;
	static const uint32_t MASK_POS = 262080; // 0b111111111111000000: mask to obtain first 12 digits with position info

	// uint8_t inputStream;
    long position_BIN;
	long position_BIN_previous;
    double position_uM;
    double positionOffset;
    double positionOffsetPole;

    std::deque<double> mData_array;
    uint8_t mData_array_interpolation_index;

};

#endif // AS5311_H