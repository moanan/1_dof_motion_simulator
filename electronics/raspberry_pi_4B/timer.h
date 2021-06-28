#ifndef TIMER_H
#define TIMER_H

#include <chrono>


class Timer
{
public:
    using clock = std::chrono::steady_clock;

    Timer() : mStart(clock::now()), mStop()
    {

    }

    ~Timer() {}

    void start()
    {
        mStart = clock::now();
    }

    double stop()
    {
        mStop = clock::now();

        return this->elapsedSeconds();
    }

    clock::duration currentDuration()
    {
        auto elapsed = clock::now() - mStart;

        return elapsed;
    }


    double currentSeconds()
    {
        auto elapsed = clock::now() - mStart;
        double seconds = std::chrono::duration<double>(elapsed).count();

        return seconds;
    }

    double elapsedSeconds()
    {
        auto elapsed = mStop - mStart;
        double seconds = std::chrono::duration<double>(elapsed).count();

        return seconds;
    }

protected:
    clock::time_point mStart;
    clock::time_point mStop;

};


#endif //TIMER_H
