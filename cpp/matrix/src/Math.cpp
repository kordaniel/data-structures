#include "Math.hpp"

#include <random>
#include <cmath>
#include <algorithm>
#include <type_traits> // is_same
#include <cassert>
#include <limits>


template<typename T> bool
Math::AreEqual(T val1, T val2)
{
    if constexpr (std::is_same<T, float>() || std::is_same<T, double>())
    {
        constexpr T EPSILON = static_cast<T>(4.0) * std::numeric_limits<T>::epsilon();
        return std::fabs(val1 - val2) < EPSILON * std::max(std::fabs(val1), std::fabs(val2));
    }
    else
    {
        static_assert(
            !std::is_same<T, int>() || !std::is_same<T, size_t>(),
            "AreEqual template type not supported"
        );

        return val1 == val2;
    }
}

template bool Math::AreEqual<float>(float, float);
template bool Math::AreEqual<double>(double, double);
template bool Math::AreEqual<int>(int, int);
template bool Math::AreEqual<size_t>(size_t, size_t);


template<typename RealT> RealT
Random::Real(RealT minInclusive, RealT maxInclusive)
{
    assert(minInclusive <= maxInclusive);
    static std::random_device os_seed;
    static std::default_random_engine generator(os_seed());
    static std::uniform_real_distribution<RealT> distribution(
        minInclusive, std::nextafter(maxInclusive, std::numeric_limits<RealT>::max())
    );

    return distribution(generator);
}

template<typename IntT> IntT
Random::Integer(IntT minInclusive, IntT maxInclusive)
{
    assert(minInclusive <= maxInclusive);
    static std::random_device os_seed;
    static std::mt19937 generator(os_seed());
    static std::uniform_int_distribution<IntT> distribution(minInclusive, maxInclusive);
    return distribution(generator);
}

template float  Random::Real<float>(float, float);
template double Random::Real<double>(double, double);
template int    Random::Integer<int>(int, int);
template size_t Random::Integer<size_t>(size_t, size_t);
