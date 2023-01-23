#include "Matrix.hpp"
#include "Io.hpp"
#include "Math.hpp"

#include <iostream>


int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    static_assert(std::is_same<size_t, unsigned long>(), "size_t is not unsigned long");

    Matrix<float> ID {
        {{ 1.0f }, { 0.0f }, { 0.0f }},
        {{ 0.0f }, { 1.0f }, { 0.0f }},
        {{ 0.0f }, { 0.0f }, { 1.0f }}
    };

    Matrix<float> A(2, 3);
    A[0][0] = .1f; A[0][1] = .2f; A[0][2] = .3f;
    A[1][0] = .4f; A[1][1] = .5f; A[1][2] = .6f;

    Matrix<float> B = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };

    std::cout << "ID:\n"    <<     ID << std::endl;
    std::cout << "A:\n"     <<      A << std::endl;
    std::cout << "B:\n"     <<      B << std::endl;
    std::cout << "A*ID:\n"  << A * ID << std::endl;
    std::cout << "B*ID:\n"  << B * ID << std::endl;
    std::cout << "ID*B:\n"  << ID * B << std::endl;
    std::cout << "A * B:\n" <<  A * B << std::endl;
    std::cout << "B^2:\n"   <<  B * B << std::endl;

    return 0;
}
