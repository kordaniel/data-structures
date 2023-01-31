#include "Timer.hpp"


Timer::Timer()
    : _start(clock::now())
{
    //
}

template<typename T> int64_t
Timer::Elapsed() const
{
    int64_t elapsed = std::chrono::duration_cast<T>(clock::now() - _start).count();
    return elapsed;
}

void
Timer::Reset()
{
    _start = clock::now();
}

template int64_t Timer::Elapsed<std::chrono::nanoseconds>() const;
template int64_t Timer::Elapsed<std::chrono::microseconds>() const;
template int64_t Timer::Elapsed<std::chrono::milliseconds>() const;
template int64_t Timer::Elapsed<std::chrono::seconds>() const;
