#include "as5311.h"

// #define DEBUG

// Ctor
AS5311::AS5311() : Sensor(COUNT_DATA)
{
	#ifdef DEBUG
		std::cout << "AS5311 object created!" << std::endl;
	#endif
	initEncoder();

	updateMagneticData();
	positionOffsetPole = 0;
	mData_array_interpolation_index = 0;
}
// Dtor
// ~AS5311() {}

void AS5311::initEncoder()
{
	if (!bcm2835_init())
    {
      std::cout << "bcm2835_init failed. Are you running as root??\n" << std::endl;
    }

	bcm2835_gpio_fsel(CLK, BCM2835_GPIO_FSEL_OUTP); 	// SCK
	bcm2835_gpio_fsel(CSn, BCM2835_GPIO_FSEL_OUTP); 	// CSn - has to toggle high and low to signal chip to start data transfer
	bcm2835_gpio_fsel(DO, BCM2835_GPIO_FSEL_INPT); 	// SDA
	bcm2835_gpio_set_pud(DO, BCM2835_GPIO_PUD_UP);
}

void AS5311::determineDirection()
{
	if((position_BIN_previous - position_BIN) > 3000)
	{
		positionOffsetPole++;
		// std::cout << "position_BIN: " << position_BIN << "\tposition_BIN_previous: " << position_BIN_previous << (position_BIN_previous - position_BIN) << std::endl;
	}
	else if((position_BIN - position_BIN_previous) > 3000)
	{
		positionOffsetPole--;
		// std::cout << "position_BIN: " << position_BIN << "\tposition_BIN_previous: " << position_BIN_previous << (position_BIN_previous - position_BIN) << std::endl;

	}
	position_BIN_previous = position_BIN;
}

void AS5311::updateMagneticData()
{
	uint32_t packedData;
	uint8_t inputStream; // one bit read from pin

	// CSn needs to cycle from high to low to initiate transfer. Then clock cycles. As it goes high
	// again, data will appear on sda
	bcm2835_gpio_write(CSn, HIGH); // CSn high
	bcm2835_gpio_write(CLK, HIGH); // CLK high

	bcm2835_delayMicroseconds(delayTime);

	bcm2835_gpio_write(CSn, LOW); // CSn low: start of transfer
	bcm2835_gpio_write(CLK, LOW); // CLK goes low: start clocking

	bcm2835_delayMicroseconds(delayTime);
	// delayMicroseconds(HALF_PERIOD); // hold low for

	for (int x = 0; x < 18; x++) // clock signal, 18 transitions, output to clock pin
	{
		bcm2835_gpio_write(CLK, HIGH); // clock goes high
		// delayMicroseconds(HALF_PERIOD); // hold low for
		bcm2835_delayMicroseconds(delayTime);  // wait 1ms cannot be removed!
		inputStream = bcm2835_gpio_lev(DO); // read one bit of data from pin
		packedData = ((packedData << 1) + inputStream); // left-shift summing variable, add pin value
		bcm2835_gpio_write(CLK, LOW);
		bcm2835_delayMicroseconds(delayTime);  // wait 1ms cannot be removed!
	} // end of entire clock cycle

	#ifdef DEBUG
    	std::cout << "packedData(BIN): " << std::bitset<18>(packedData) << "\tposition_uM: " << position_uM << "\tpositionOffsetPole: " << positionOffsetPole << std::endl;
	#endif
	// calculate linear position
	position_BIN = (packedData & MASK_POS) >> 6; // mask rightmost 6 digits of packedData to zero, into 12-bit position
}

measureData AS5311::measure()
{
	// init
	initMeasurement();

	updateMagneticData();
	determineDirection();
	position_uM = (position_BIN - positionOffset) * 448.28125 / 1000.0 + positionOffsetPole * 2000; //
    mData[1] = position_uM;

	mData_array_interpolation_index++;
	if(aerial && (mData_array_interpolation_index%10 == 0))	//	reduce array size by factor of 5
	{
		mData_array.push_back(position_uM);
		mData_array_interpolation_index = 0;
	}

	if(thrust_on)	// find max when the leg is at low vertical speed.
	{
		thrust_on = false;
		std::deque<double>::iterator it = std::min_element(mData_array.begin(), mData_array.end());
		// std::cout << "Array size: " << mData_array.size() << std::endl;
		apex_previous = *it;
		mData_array.clear();
		if(debug_msg == 4)
		{
			// std::cout << "Last apex position [mm]: " << apex_previous*0.001 << "	Current [A]: " << currentSatuation << std::endl;
			std::cout << apex_previous*0.001 << "\t" << currentSatuation << std::endl;
		}
	}

    if(debug_msg == 3)
	{
		print(mData, 100, 1);	// print encoder readings
	}

	return mData;
}