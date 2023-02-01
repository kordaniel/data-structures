#include "Matrix.hpp"
#include "Io.hpp"
#include "Math.hpp"
#include "Timer.hpp"
#include "ThreadPool.hpp"

#include <functional>
#include <iostream>
#include <memory>

#define DO_TESTS 1

#ifdef DO_TESTS
    #include <cstdlib>
    #define TEST_DT float
    // Specify the datatype to use in the tests
    // from the set of: float, double and int.
#endif


int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    static_assert(std::is_same<size_t, unsigned long>(), "size_t is not unsigned long");

    size_t numThreads = 4;

#ifdef DO_TESTS
    if (argc > 1) {
        numThreads = strtoul(argv[1], nullptr, 10);
    }
#endif

    ThreadPool threadPool(numThreads, true); // Initialize and start threadpool, matrix class will use worker
                                             // threads with large enough matrices to gain benefits from parallelism.

#ifdef DO_TESTS

    int64_t elapsed;
    int64_t total = 0;
    Matrix<TEST_DT>* BCptr = nullptr;

    std::cout << "Running timing tests for generation and multiplication of matrices.\n"
              << "All printed times are real wall clock times of the different\n"
              << "opearations and are reported in milliseconds. Do note that these\n"
              << "times might differ from the times reported by different tools, since\n"
              << "this is a multithreaded application.\n"
              << "Using: " << threadPool.GetThreadsCount() << " worker threads.\n"
              << "--------------------------------------------------------------------\n" << std::endl;

    Timer tim;

    {
        tim.Reset();
        Matrix<TEST_DT> B = Matrix<TEST_DT>::Random(
            1000, 1'000'000,
            std::bind<TEST_DT>(&Random::Fast<TEST_DT>, static_cast<TEST_DT>(0.0), static_cast<TEST_DT>(1.0))
        );
        elapsed = tim.Elapsed<std::chrono::milliseconds>();
        total += elapsed;
        std::cout << "Generated B in: " << elapsed << " ms." << std::endl;

        tim.Reset();
        Matrix<TEST_DT> C = Matrix<TEST_DT>::Random(
            1'000'000, 1,
            std::bind<TEST_DT>(&Random::Fast<TEST_DT>, static_cast<TEST_DT>(0.0), static_cast<TEST_DT>(1.0))
        );
        elapsed = tim.Elapsed<std::chrono::milliseconds>();
        total += elapsed;
        std::cout << "Generated C in: " << elapsed << " ms." << std::endl;

        tim.Reset();
        auto BC = B * C;
        elapsed = tim.Elapsed<std::chrono::milliseconds>();
        total += elapsed;
        std::cout << "Computed BC in: " << elapsed << " ms." << std::endl;

        BCptr = &BC;
    }

    if (BCptr == nullptr) {
        std::cerr << "No ptr, lost matrix BC.." << std::endl;
        return 1;
    }

    tim.Reset();
    Matrix<TEST_DT> A = Matrix<TEST_DT>::Random(
        1'000'000, 1000,
        std::bind<TEST_DT>(&Random::Fast<TEST_DT>, static_cast<TEST_DT>(0.0), static_cast<TEST_DT>(1.0))
    );
    elapsed = tim.Elapsed<std::chrono::milliseconds>();
    total += elapsed;
    std::cout << "Generated A in: " << elapsed << " ms." << std::endl;

    tim.Reset();
    auto ABC = A * *BCptr;
    elapsed = tim.Elapsed<std::chrono::milliseconds>();
    total += elapsed;
    std::cout << "Computed A*BC in: " << elapsed << " ms.\n"
              << "---------------------------------------\n"
              << "In total: " << total << " ms." << std::endl;

    #undef TEST_DT

#else

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

#endif

    return 0;
}
