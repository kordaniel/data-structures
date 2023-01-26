#include "Math.hpp"

#include <random>
#include <cmath>
#include <algorithm>
#include <type_traits> // is_same
#include <cassert>
#include <limits>


template<typename T> bool
Math::AreEqual(const T& val1, const T& val2, [[maybe_unused]] const T realTypeToleranceFactor)
{
    if constexpr (std::is_same<T, float>() || std::is_same<T, double>())
    {
        const T EPSILON = realTypeToleranceFactor * std::numeric_limits<T>::epsilon();
        return std::fabs(val1 - val2) < EPSILON * std::max({ static_cast<T>(1.0), std::fabs(val1), std::fabs(val2) });
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

template bool Math::AreEqual<float>(const float&, const float&, const float);
template bool Math::AreEqual<double>(const double&, const double&, const double);
template bool Math::AreEqual<int>(const int&, const int&, const int);
template bool Math::AreEqual<size_t>(const size_t&, const size_t&, const size_t);


template<typename T> T
Random::UniformlyDistributed(T minInclusive, T maxInclusive)
{
    assert(minInclusive < maxInclusive); // if ==, then undefined behaviour for real types!
    static std::random_device os_seed;
    static std::mt19937 generator(os_seed());

    if constexpr (std::is_same<T, float>() || std::is_same<T, double>())
    {
        static std::uniform_real_distribution<T> distribution(
            minInclusive, std::nextafter(maxInclusive, std::numeric_limits<T>::max())
        );
        return distribution(generator);
    }
    else if constexpr (std::is_same<T, int>() || std::is_same<T, size_t>())
    {
        static std::uniform_int_distribution<T> distribution(minInclusive, maxInclusive);
        return distribution(generator);
    }
    else
    { // NOTE: If (only) the supported template types are defined at the last rows of this module, then
      //       the compiler should never branch here, so this is just a sanity check (to silence compiler).
        static_assert(!sizeof(T*), "Random::UniformlyDistributed unsupported template type");
    }
}

template<typename T> T
Random::Fast(T minInclusive, T maxInclusive)
{
    assert(minInclusive <= maxInclusive);
    static std::random_device os_seed;
    static std::minstd_rand generator(os_seed());

    if constexpr (std::is_same<T, float>() || std::is_same<T, double>())
    {
        return minInclusive +
            (static_cast<T>(generator()) /
            (static_cast<T>(generator.max()) / (maxInclusive - minInclusive)));
    }
    else if constexpr (std::is_same<T, int>())
    {
        return minInclusive +
            (static_cast<T>(generator()) % (maxInclusive - minInclusive + static_cast<T>(1)));
    }
    else
    { // NOTE: If (only) the supported template types are defined at the last rows of this module, then
      //       the compiler should never branch here, so this is just a sanity check (to silence compiler).
        static_assert(!sizeof(T*), "Random::Fast unsupported template type");
    }
}


template float  Random::UniformlyDistributed<float>(float, float);
template double Random::UniformlyDistributed<double>(double, double);
template int    Random::UniformlyDistributed<int>(int, int);
template size_t Random::UniformlyDistributed<size_t>(size_t, size_t);

template float  Random::Fast<float>(float, float);
template double Random::Fast<double>(double, double);
template int    Random::Fast<int>(int, int);
