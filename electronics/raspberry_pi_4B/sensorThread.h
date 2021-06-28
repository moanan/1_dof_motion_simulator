#ifndef ACTUATION_H
#define ACTUATION_H

#include <string>
#include <thread>
#include <cstdint>
#include <atomic>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "timer.h"
#include "sensor.h"
#include "as5045.h"


class SensorThread
{
public:
    // Ctor
    SensorThread(std::string name, Sensor& measurement, uint16_t frequency, bool& running);
    // Dtor
    ~SensorThread()
    {
        // mRunning = false;
        if(mThread.joinable())
            mThread.join();
    }

    void start();
    void join();


protected:
    std::string mName;
    Sensor& mMeasurement;
    std::thread mThread;
    bool* mRunning;
    std::chrono::duration<double> mPeriod;
    std::ofstream mFile;
    char fileName[200];
    const char *ext = ".csv"; // FILE EXTENSION to OPEN OUTPUT FILE IN REQUIRED FORMAT
    // int printIndex;

    void run();
    // bool print(const measureData& data, int inteval);
    bool publish(const measureData& data);
    void initFileName();

};


#endif //ACTUATION_H
