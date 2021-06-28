#include "sensor.h"
#include <chrono>


// Ctor
Sensor::Sensor() : printIndex(0)
{

}

double Sensor::getCurrentTime()
{
    // Get steady clock time
    using clock = std::chrono::steady_clock;
    auto currentTime = clock::now().time_since_epoch();

    // Convert to double representation
    double seconds = std::chrono::duration<double>(currentTime).count();

    return seconds;
}

long Sensor::getCurrentTimeMillis()
{
	long millis = (long)(1000 * getCurrentTime());
	return millis;
}

void Sensor::initMeasurement()
{
    std::fill(mData.begin(), mData.end(), 0.);
    mData[0] = getCurrentTime();
}

bool Sensor::print(const measureData& data, int inteval, int precision)
{
    printIndex++;
    if(printIndex >= inteval)
    {
        std::cout << std::fixed << std::showpoint;
        std::cout << std::setprecision(precision); 
        for (auto i = data.begin(); i != data.end(); ++i) 
        {
            std::cout << *i << "\t";
        }
        std::cout << std::endl;
        printIndex = 0;
        return true;
    }
    return false;
} 