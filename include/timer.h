#ifndef TIMER_H
#define TIMER_H
#include <chrono>

class Timer
{
public:
    Timer(){};
    ~Timer(){};
    void Start();
    std::chrono::milliseconds Stop();
    void Reset();
private:
    std::chrono::time_point<std::chrono::steady_clock> time_start_;
};

#endif /* TIMER_H */
