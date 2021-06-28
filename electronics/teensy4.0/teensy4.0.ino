/*
LSO oscillator
Platform: Teensy 4.0
------------------
created 2020-11-03
by An Mo

compile:
arduino 1.8.13 + teensydruino

version 1.0
*/

using namespace std;

#include <vector>
#include <algorithm>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define STEP 23
#define DIR 22
#define DISABLE 21
#define UP 3
#define DOWN 6
#define FREQUENCY_SELECT 9
#define START 10
#define led 13
LiquidCrystal_I2C lcd(0x3f,16,2); // depends on your LCD, could be 0x27


// set configuration parameters
int state = 0;
int microStepping = 10;
int manualDelayUs = 100;
int targetTriggerState = 1; // 0: idle; 1: oscillate; 2: test function.
int frequency_index = 0;

// set simulation parameters - oscillate
int amplitude = 10;				// [mm]
double frequency = 2.0;			// [Hz], default frequency at boot
double frequencies[] = {3.0, 3.5, 4.0, 4.5};	// [Hz]
double oscillator_on_s = 5; 	// [s]
double oscillator_off_s = 3; 	// [s]
int repetition = 8;				// repeat the protocol for a given number of repetitions

vector<long> generateDelayVector_us(int steps, double frequency)
{
	// generate microsecond delay between steps

	// Arcsine time calculation to fill timeAtStep
	long timeAtStep[steps];
	double positionAtStep = -1;
	double positionAtStep_diff = 2.0/steps;
	for(int i = 0; i <= steps; i++)
	{
		timeAtStep[i] = round(asin(positionAtStep)/(2*PI*frequency)*1000000);
		positionAtStep += positionAtStep_diff;
	}
	
	// Calculating the time difference between each steps to recieve delay value
	std::vector<long> timeAtStep_diff;
	for(int i = 0; i < steps; i++)
	{
		timeAtStep_diff.push_back((timeAtStep[i+1] - timeAtStep[i]));
	}
 	return timeAtStep_diff;
}

void detectButton()
{
	if(!digitalRead(UP))
	{
		lcd.clear();lcd.home();
		lcd.print("Moving up...");
		digitalWriteFast(DISABLE, LOW);	// enable stepper motor
		digitalWriteFast(DIR, LOW);
		while(!digitalRead(UP))
		{
			digitalWriteFast(STEP, HIGH);
			delayMicroseconds(manualDelayUs);
			digitalWriteFast(STEP, LOW);
			delayMicroseconds(manualDelayUs);
		}
		digitalWriteFast(DISABLE, HIGH);	// disable stepper motor
	}

	if(!digitalRead(DOWN))
	{
		lcd.clear();lcd.home();
		lcd.print("Moving down...");
		digitalWriteFast(DISABLE, LOW);	// enable stepper motor
		digitalWriteFast(DIR, HIGH);
		while(!digitalRead(DOWN))
		{
			digitalWriteFast(STEP, HIGH);
			delayMicroseconds(manualDelayUs);
			digitalWriteFast(STEP, LOW);
			delayMicroseconds(manualDelayUs);
		}
		digitalWriteFast(DISABLE, HIGH);	// disable stepper motor
	}

	if(!digitalRead(FREQUENCY_SELECT))
	{
		delay(60); // debouce time
		if(!digitalRead(FREQUENCY_SELECT))
		{
			int array_size = sizeof(frequencies)/sizeof(double);
			if(frequency_index < array_size - 1)
			{
				frequency_index++;
			}
			else
			{
				frequency_index = 0;
			}
			frequency = frequencies[frequency_index];
			Serial.print("frequency_index: ");
			Serial.println(frequency_index);
		}
	}

	if(!digitalRead(START))
	{
		state = targetTriggerState;
	}
}

// the setup routine runs once when you press reset:
void setup() 
{
	pinMode(led, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(DIR, OUTPUT);
	pinMode(DISABLE, OUTPUT);
	pinMode(UP, INPUT_PULLUP);
	pinMode(DOWN, INPUT_PULLUP);
	pinMode(FREQUENCY_SELECT, INPUT_PULLUP);
	pinMode(START, INPUT_PULLUP);

	digitalWriteFast(DISABLE, HIGH);
	digitalWriteFast(STEP, LOW);
	digitalWriteFast(DIR, LOW);

	lcd.init();
	lcd.backlight();
	Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() 
{
	
	switch(state)
	{
	case 0:	// manual move while waiting for trigger
		{
			Serial.println("Waiting for trigger...");
			lcd.setCursor(0,0);
			lcd.print("LSS test stand");
			lcd.setCursor(0,1);
			lcd.print("Freq:");
			lcd.setCursor(5,1);
			lcd.print(frequency);
			lcd.setCursor(8,1);
			lcd.print(" Amp:");
			lcd.setCursor(13,1);
			lcd.print(amplitude);			

			detectButton();
		}
		break;

	case 1:	// oscillate
		{
			// init
			int cycles = (int)ceil(oscillator_on_s * frequency) * 2;
			int step = 0;			// for steps
			int cycle = 0; 			// for switching direction
			int steps = amplitude*(200*microStepping/5);
			vector<long> delay_us = generateDelayVector_us(steps, frequency);
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("freq: ");
			lcd.setCursor(7,0);
			lcd.print(frequency);
			lcd.setCursor(0,1);
			lcd.print("repetition: ");
			// lcd.setCursor(12,1);
			// lcd.print(repetition);			
			Serial.print(steps);Serial.print(" steps generated for ");Serial.print(amplitude);Serial.print(" [mm] stroke, with delay ");Serial.print(*min_element(delay_us.begin(), delay_us.end()));Serial.print("-");Serial.print(*max_element(delay_us.begin(), delay_us.end()));Serial.println(" [us] (valid: 1-16000 [us]) between steps");
			
			digitalWriteFast(DISABLE, LOW);	// enable stepper motor
			lcd.noBacklight();
			delay(2000);				// backlight off and on for video timing

			for(int i = 0; i < repetition; i++)
			{
				lcd.setCursor(12,1);
				lcd.print(i+1);
				// start oscillating
				digitalWriteFast(DIR, HIGH);
				lcd.backlight();
								
				while(cycle < cycles)
				{
					digitalWriteFast(STEP, HIGH);
					delayMicroseconds(delay_us.at(step)/2);
					digitalWriteFast(STEP, LOW);
					delayMicroseconds(delay_us.at(step)/2);

					step++;
					if(step >= steps)
					{
						step = 0;
						cycle++;
						digitalWriteFast(DIR, !digitalRead(DIR));
					}
				}

				lcd.noBacklight();
				cycle = 0;
				step = 0;
				delay(oscillator_off_s*1000);
			}

			Serial.println("Oscillation terminated!");
			digitalWriteFast(DISABLE, HIGH);	// disable stepper motor
			lcd.backlight();
			state = 0;
		}
		break;

		case 2: // test state: LCD
		{
			lcd.backlight();
			//Nachricht ausgeben
			lcd.setCursor(0,0);
			lcd.print(" joy-IT");
			lcd.setCursor(0,1);
			lcd.print(" I2C Serial LCD");
			state = 0;
		}

	}
}