/*

Modular multi-thread high-speed sensor reading

Platform: Raspberry Pi 3/4
------------------
modified 2021-06-22
modified 2020-11-06
created  2020-03-12

by An Mo

version 1.1

*/

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <cstring>
#include <atomic>

#include "sensorThread.h"

std::atomic<bool> quit(false);
bool as5045Reading = false;

void intHandler(int)	// execute when you 'Ctrl+C'
{
	quit.store(true);	// reserved for while loop in main.cpp
	as5045Reading = false;

	std::cout << " Ctrl+C triggered. Program exit." << std::endl;
}

int main()
{
	// make sure the destructors get called when the program is killed
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = intHandler;
	sigfillset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);

	// init
	uint16_t rotaryEncoderFrequency = 2000;  // [Hz]
	AS5045 as5045;  //initialize AS5045 instance
	SensorThread sensorThread("AS5045-", as5045, rotaryEncoderFrequency, as5045Reading);

	sensorThread.start();
	sensorThread.join();
	
	std::cout << "Program closed." << std::endl;
	return(0);
}