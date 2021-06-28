#include "sensorThread.h"

// #define DEBUG

SensorThread::SensorThread(std::string name, Sensor& measurement, uint16_t frequency, bool& running) :
    mName(name),
    mMeasurement(measurement),
    mThread(),
    mRunning(&running),
    mPeriod(std::chrono::duration<double>(1. / frequency))
{
    
}

void SensorThread::start()
{
    *mRunning = true;
    
    // prepare file handler
    initFileName();
    mFile.open(fileName, std::ofstream::app);
    mFile << "time [s],displacement [mm],angle [12bit]\n"; // headline of .csv datafile
    if (!mFile.is_open())
    {
        std::cerr << "Data file not open! Please check your file path!" << std::endl;
    }

    mThread = std::thread(&SensorThread::run, this);
}

void SensorThread::run()
{
    using namespace std::chrono;

    #ifdef DEBUG
        std::cout << mName << ": thread is running..." << std::endl;
    #endif
    
    auto const startTime = steady_clock::now();
    auto nextTime = startTime + mPeriod; // regularly updated time point

#ifdef DEBUG
    Timer t;
#endif
    while(*mRunning) // Main thread loop
    {
#ifdef DEBUG
        t.start();
#endif
     // Send control command and receive data
        auto data = mMeasurement.measure();
        // print(data, 10);
        publish(data);

#ifdef DEBUG
        t.stop();
        if(std::chrono::duration<double>(t.currentSeconds()) > mPeriod)
            std::cerr << mName << ": " << 1. / t.currentSeconds() << " Hz" << std::endl;
#endif

        // Sleep for other threads
        std::this_thread::sleep_until(nextTime);
        nextTime += mPeriod; // increment absolute time
    }
    mFile.close();
    std::cout << "File closed." << std::endl;
}


bool SensorThread::publish(const measureData& data)
{
    mFile << std::fixed << std::setprecision(4);
    for (auto i = data.begin(); i != data.end(); ++i) 
    {
        mFile << *i << ",";
    }
    mFile << "\n";
    return true;
}

void SensorThread::join()
{
    mThread.join();
}

void SensorThread::initFileName()
{
  time_t     now = time(0);
  struct tm  tstruct;
  tstruct = *localtime(&now);
  std::fill_n(fileName, 200, 0);
  char tempName[200];
  strftime(tempName, sizeof(tempName), "%Y-%m-%d-%H-%M-%S", &tstruct);
  strcat(fileName, "./rawData/");      // make sure the folder exist!!
  char *cmName = &mName[0];
  strcat(fileName, cmName);  // add data name 
  strcat(fileName, tempName); // add timestamp
  strcat(fileName, ext); // add file extension
  // std::cout << "file name: " << fileName << std::endl;
}

