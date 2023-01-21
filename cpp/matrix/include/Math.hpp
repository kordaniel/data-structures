#ifndef MATH_HPP
#define MATH_HPP


namespace Math
{

    template<typename T> bool
    AreEqual(T val1, T val2);

} // end namespace Math

namespace Random
{
    template<typename RealT> RealT
    Real(RealT minInclusive = 0.0, RealT maxInclusive = 1.0);

    template<typename IntT> IntT
    Integer(IntT minInclusive, IntT maxInclusive);

} // end namespace Random


#endif // MATH_HPP
