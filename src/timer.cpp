#include "timer.h"
void Timer::Start()
{
    time_start_ = std::chrono::steady_clock::now();
}

std::chrono::milliseconds Timer::Stop()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_start_);
}

void Timer::Reset()
{
    time_start_ = std::chrono::steady_clock::now();
};

