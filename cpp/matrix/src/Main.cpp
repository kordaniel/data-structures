#include "Matrix.hpp"
#include "Io.hpp"
#include "Math.hpp"
#include "ThreadPool.hpp"

#include <functional>
#include <iostream>


int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    static_assert(std::is_same<size_t, unsigned long>(), "size_t is not unsigned long");

    ThreadPool threadPool(4, true); // Initialize and start threadpool, matrix class will use worker
                                    // threads with large enough matrices to gain benefits from parallelism.

    std::cout << "Identity matrices with diagonal sizes in [1,3] and with types in [float,double,int]:" << std::endl;
    for (size_t i = 1; i < 4; ++i) {
        std::cout << Matrix<double>::ID(i) << std::endl;
        std::cout << Matrix<float>::ID(i) << std::endl;
        std::cout << Matrix<int>::ID(i) << std::endl << std::endl;
    }
    std::cout << "Matrices with float type and multiplications of them:" << std::endl;
    Matrix<float> ID {
        {{ 1.0f }, { 0.0f }, { 0.0f }},
        {{ 0.0f }, { 1.0f }, { 0.0f }},
        {{ 0.0f }, { 0.0f }, { 1.0f }}
    };

    Matrix<float> A(2, 3);
    A[0][0] = .1f; A[0][1] = .2f; A[0][2] = .3f;
    A[1][0] = .4f; A[1][1] = .5f; A[1][2] = .6f;

    Matrix<float> B({
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    }, Matrix<float>::Ordering::ColumnMajor);

    std::cout << "ID:\n"      <<     ID << std::endl;
    std::cout << "A:\n"       <<      A << std::endl;
    std::cout << "B:\n"       <<      B << std::endl;
    std::cout << "A*ID:\n"    << A * ID << std::endl;
    std::cout << "B*ID:\n"    << B * ID << std::endl;
    std::cout << "ID*B:\n"    << ID * B << std::endl;
    std::cout << "A * B:\n"   <<  A * B << std::endl;
    std::cout << "B^2:\n"     <<  B * B << std::endl;
    std::cout << "B^2 + B:\n" <<  (B * B) + B << std::endl;
    std::cout << "B^2 - B:\n" <<  (B * B) - B << std::endl;
    std::cout << "B-B:\n"     <<  B - B << std::endl;

    std::cout << "Randomly generated matrices:" << std::endl;
    Matrix<float>  R1 = Matrix<float>::Random(2, 2, std::bind<float>(&Random::Fast<float>, 0.0f, 1.0f), Matrix<float>::Ordering::ColumnMajor);
    Matrix<double> R2 = Matrix<double>::Random(5, 10, std::bind<double>(&Random::Fast<double>, 10.0, 10.5));
    Matrix<int>    R3 = Matrix<int>::Random(
        7, 3,
        std::bind(&Random::Fast<int>, 0, 1),
        Matrix<int>::Ordering::ColumnMajor
    );
    std::cout << "R1, float vals in [0,1]:\n" << R1 << std::endl;
    std::cout << "R2, double vals in [10.0, 10.5]:\n" << R2 << std::endl;
    std::cout << "R3, int vals in [0,1]:\n" << R3 << std::endl;
    std::cout << "R3_(5,2): " << R3[5][2] << std::endl;

    return 0;
}
