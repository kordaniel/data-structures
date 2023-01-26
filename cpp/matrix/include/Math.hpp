#ifndef MATH_HPP
#define MATH_HPP


namespace Math
{

    /// @brief Compares two values of type T for equality.
    /// @tparam T must be one of float, double, int or size_t.
    /// @param val1 value to compare.
    /// @param val2 value to compare.
    /// @param realTypeToleranceFactor Factor to set tolerance, default value is 1. This factor is used to multiplicate std::numeric_limits<T>::epsilon() for RealTypes.
    /// @return true if values are considered equal, false otherwise.
    template<typename T> bool
    AreEqual(const T& val1, const T& val2, const T realTypeToleranceFactor = static_cast<T>(1));

} // end namespace Math

namespace Random
{
    template<typename T> T
    UniformlyDistributed(T minInclusive = static_cast<T>(0), T maxInclusive = static_cast<T>(1));

    /// Returns a random value from the inclusive range [minInclusive, maxInclusive].
    /// The returned values are not distributed in any particular way and the used
    /// pseudo-random generator is optimized for speed over randomness.
    template<typename T> T
    Fast(T minInclusive = static_cast<T>(0), T maxInclusive = static_cast<T>(1));

} // end namespace Random


#endif // MATH_HPP
