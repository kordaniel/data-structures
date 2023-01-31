#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>


class Timer
{
    using clock = std::chrono::steady_clock;

public:
    Timer();

    /// @brief Counts and returns the real time passed since the instantiation
    ///        of this object or since the previous call of the Reset method.
    /// @tparam T std::chrono::[nano|micro|milli|'']seconds
    /// @return The elapsed time in units specified by the template argument.
    template<typename T>
    int64_t Elapsed() const;

    void Reset();

private:
    std::chrono::time_point<clock> _start;

};


#endif // TIMER_HPP
