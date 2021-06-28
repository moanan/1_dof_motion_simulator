/*

Reading encoder AS5045 with SSI
Platform: Raspberry Pi 3/4
------------------
created 2020-11-06
by An Mo

version 0.9

Important:

SSI data format:
+----------+-----------+
|   data   |   status  |
+----------+-----------+
| 12 bits  |   6 bits  |
+----------+-----------+

The internal sampling frequency of AS5045 is 2.61 kHz.

*/
#include <iostream>
#include <vector>

#include "as5045.h"

// #define DEBUG

// Ctor
AS5045::AS5045() : Sensor(COUNT_DATA)
{
  #ifdef DEBUG
    std::cout << "AS5045 object created!" << std::endl;
  #endif
  initEncoder();
} 
// Dtor
// ~AS5045() {}

void AS5045::initEncoder()
{
  if (!bcm2835_init())
    {
      std::cout << "bcm2835_init failed. Are you running as root??\n" << std::endl;
    }

  bcm2835_gpio_fsel(CLK, BCM2835_GPIO_FSEL_OUTP);   // SCK
  bcm2835_gpio_fsel(CSn, BCM2835_GPIO_FSEL_OUTP);   // CSn - has to toggle high and low to signal chip to start data transfer
  bcm2835_gpio_fsel(DO, BCM2835_GPIO_FSEL_INPT);  // SDA
  bcm2835_gpio_set_pud(DO, BCM2835_GPIO_PUD_UP);

  updateMagneticData();
  angleOffset = angle_raw;
}

void AS5045::determineDirection()
{
  if((angle_raw_previous - angle_raw) > 3000)
  {
    turns++; 
    // std::cout << "angle_raw: " << angle_raw << "\tangle_raw_previous: " << angle_raw_previous << (angle_raw_previous - angle_raw) << std::endl;
  }
  else if((angle_raw - angle_raw_previous) > 3000)
  {
    turns--;
    // std::cout << "angle_raw: " << angle_raw << "\tangle_raw_previous: " << angle_raw_previous << (angle_raw_previous - angle_raw) << std::endl;

  }
  angle_raw_previous = angle_raw;
}

void AS5045::updateMagneticData() 
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
  angle_raw = (packedData & MASK_POS) >> 6; // mask rightmost 6 digits of packedData to zero, into 12-bit position
} 

measureData AS5045::measure()
{
  // init
  initMeasurement();
  updateMagneticData();
  determineDirection();

  position_mm = (double)(angle_raw - angleOffset + 4095 * turns) * TO_DISPLACEMENT; 
  mData[1] = position_mm;
  // mData[2] = angle_raw;

  print(mData, 20, 2); // print encoder readings
  return mData;
}



