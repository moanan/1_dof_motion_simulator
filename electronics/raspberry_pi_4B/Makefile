# declare the variable
CFLAGS=-c -Wall -Wextra -g -Wno-psabi

# this target will compile all the files
all: encoder

encoder: main.o sensorThread.o sensor.o as5045.o
	g++ -Wall -o encoder main.o sensorThread.o sensor.o as5045.o -lbcm2835 -pthread

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

sensorThread.o: sensorThread.cpp
	g++ $(CFLAGS) sensorThread.cpp

sensor.o: sensor.cpp
	g++ $(CFLAGS) sensor.cpp

as5045.o: as5045.cpp
	g++ $(CFLAGS) as5045.cpp -lbcm2835

clean:
	rm -rf *o encoder