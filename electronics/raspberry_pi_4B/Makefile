# declare the variable
CFLAGS=-c -Wall -Wextra -g -Wno-psabi

# this target will compile all the files
all: simulation_actuated

simulation_actuated: actuated.o sensorThread.o sensor.o as5045.o
	g++ -Wall -o simulation_actuated actuated.o sensorThread.o sensor.o as5045.o -lbcm2835 -pthread

simulation_manual: manual.o sensorThread.o sensor.o as5311.o
	g++ -Wall -o simulation_actuated manual.o sensorThread.o sensor.o as5311.o -lbcm2835 -pthread

actuated.o: actuated.cpp
	g++ $(CFLAGS) actuated.cpp

manual.o: manual.cpp
	g++ $(CFLAGS) manual.cpp

sensorThread.o: sensorThread.cpp
	g++ $(CFLAGS) sensorThread.cpp

sensor.o: sensor.cpp
	g++ $(CFLAGS) sensor.cpp

as5045.o: as5045.cpp
	g++ $(CFLAGS) as5045.cpp -lbcm2835

clean:
	rm -rf *o simulation_actuated simulation_manual