#pragma once

#include <chrono>

// CPU timer.
class CPUTimer {
    public:
        // Constructor.
        CPUTimer(float& dt)
        {
            mStart = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            mDt = &dt;
        }

        // Destructor.
        ~CPUTimer()
        {
            long long nanoTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - mStart;
            *mDt = static_cast<float>(nanoTime) / 1000000000.f;
        }

    private:
        long long mStart;
        float* mDt;

};

#define CPUTIMER(dt) CPUTimer cpuTimer(dt)
