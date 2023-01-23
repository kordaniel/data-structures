#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

#include "Io.hpp"
#include "Math.hpp"
#include <Matrix.hpp>


template<typename T>
std::vector<T> SplitStrToTypeVals(const std::string& str, char delimiter)
{
    std::stringstream strsequence(str);
    std::string segment;
    std::vector<T> segments;

    while (std::getline(strsequence, segment, delimiter))
    {
        if constexpr (std::is_same<T, float>::value) {
            segments.push_back(std::stof(segment));
        } else if constexpr (std::is_same<T, double>::value) {
            segments.push_back(std::stod(segment));
        } else if constexpr (std::is_same<T, int>::value) {
            segments.push_back(std::stoi(segment));
        } else if constexpr (std::is_same<T, size_t>::value) {
            segments.push_back(std::stoul(segment));
        } else {
            throw std::domain_error("SplitStrToTypeVals specified template type not supported");
        }
    }

    return segments;
}


class MatrixTest : public ::testing::Test
{
protected:

    inline static std::map<size_t, std::array<Matrix<float>, 3>> SquareMatrices;
    inline static std::vector<std::array<Matrix<float>, 3>>      RandomMatrices;

    //void SetUp() override {} // Is called before each test
    //static void TearDownTestSuite() {}

    static void SetUpTestSuite()
    { // Is called once for the whole test suite
        auto cmpByFilename = [](const IO::FilePath& a, const IO::FilePath& b) {
            return a.Filename.compare(b.Filename) < 0 ? true : false;
        };

        const std::string path = "./test/data";
        std::optional<std::vector<IO::FilePath>> filesOptional = IO::FilesInDirectory(path);
        if (!filesOptional.has_value() || filesOptional->empty()) {
            //std::cerr << "No files found in path '" + path + "'..." << std::endl;
            //std::cerr << "Create the test data by running: cd test/ && ./generate-test-matrices.py";
            throw std::ios_base::failure(
                "Initialize the test data by running: \"cd test/ && ./generate-test-matrices.py\"\n\t"
            );
        }

        std::vector<IO::FilePath> squareMatrices;
        std::vector<IO::FilePath> randomMatrices;
        for (const auto& testCase : *filesOptional)
        {
            if (testCase.Filename.find("square", 0) == 0) {
                squareMatrices.push_back(testCase);
            } else {
                randomMatrices.push_back(testCase);
            }
        }
        std::sort(squareMatrices.begin(), squareMatrices.end(), cmpByFilename);
        std::sort(randomMatrices.begin(), randomMatrices.end(), cmpByFilename);

        LoadSquareMatrices(squareMatrices);
        LoadRandomSizedMatrices(randomMatrices);

        if (SquareMatrices.size() != RandomMatrices.size()) {
            std::cout << "[WARNING]: Count of read square and random size matrices differ" << std::endl;
        }
    }

    static void LoadSquareMatrices(const std::vector<IO::FilePath>& matrixFiles)
    {
        //for (std::vector<IO::FilePath>::iterator i = matrixFiles.begin(); i < matrixFiles.begin() + 2; i++)
        for (const IO::FilePath& testCase : matrixFiles)
        {
            //IO::FilePath& testCase = *i;
            std::string fpath = testCase.Path + '/' + testCase.Filename;
            auto fileLinesOptional = IO::ReadLinesTextFile(fpath);
            if (!fileLinesOptional.has_value() || fileLinesOptional->empty()) {
                std::cerr << "Could not read file: '" << fpath << "'. Skipping file." << std::endl;
                continue;
            }
            std::vector<std::string> fileLines = *fileLinesOptional;

            std::vector<size_t> dimensions = SplitStrToTypeVals<size_t>(fileLines[0].substr(2), 'x');

            Matrix<float> A(dimensions[0], dimensions[1]);
            Matrix<float> B(dimensions[0], dimensions[1]);
            Matrix<float> C(dimensions[0], dimensions[1]);

            size_t r = 0;
            size_t m = 0;

            std::for_each(fileLines.begin() + 1, fileLines.end(), [&dimensions, &r, &m, &A, &B, &C, &fpath](const std::string& line)
            {
                std::vector<float> vals = SplitStrToTypeVals<float>(line, ' ');

                for (size_t c = 0; c < vals.size(); ++c)
                {
                    switch (m)
                    {
                        case 0: A[r][c] = vals[c]; break;
                        case 1: B[r][c] = vals[c]; break;
                        case 2: C[r][c] = vals[c]; break;
                        default: throw std::domain_error("Invalid data for square matrix: '" + fpath + "'.");
                    }
                }

                if (++r % dimensions[0] == 0)
                {
                    r = 0;
                    ++m;
                }
            });

            SquareMatrices.insert({ dimensions[0], { A, B, C } });
        }
    }

    static void LoadRandomSizedMatrices(const std::vector<IO::FilePath>& matrixFiles)
    {
        auto parseTestCaseName = [](const std::string& filename) {
            return filename.substr(0, filename.size()-2);
        };

        auto filePathToStr = [](const IO::FilePath& fp) {
            return fp.Path + '/' + fp.Filename;
        };

        std::array<std::string, 3> testCaseFiles;
        std::string currentTestCase;

        for (std::vector<IO::FilePath>::const_iterator it = matrixFiles.begin();
             it < matrixFiles.end(); )
        {
            currentTestCase = parseTestCaseName(it->Filename);
            std::vector<Matrix<float>> testCase;
            size_t matIdx = 0;

            while (parseTestCaseName(it->Filename) == currentTestCase && it < matrixFiles.end()) {
                testCaseFiles[matIdx++] = filePathToStr(*it);
                it++;
            }

            if (matIdx != testCaseFiles.size()) {
                std::cerr << "Error reading test case: '" << currentTestCase << "'. Skipping." << matIdx << std::endl;
                continue;
            }

            for (size_t m = 0; m < 3; ++m)
            {
                auto fileLinesOptional = IO::ReadLinesTextFile(testCaseFiles[m]);
                if (!fileLinesOptional.has_value() || fileLinesOptional->empty()) {
                    std::cerr << "Could not read file: '" << testCaseFiles[m] << "'. Skipping test case." << std::endl;
                    break; // TODO: Exit outer loop
                }
                std::vector<std::string> fileLines = *fileLinesOptional;

                std::vector<size_t> dimensions = SplitStrToTypeVals<size_t>(fileLines[0].substr(2), 'x');
                std::vector<float> matrixData;
                matrixData.reserve(dimensions[0] * dimensions[1]);

                for (size_t r = 1; r < fileLines.size(); ++r)
                {
                    std::vector<float> vals = SplitStrToTypeVals<float>(fileLines[r], ' ');
                    for (size_t c = 0; c < vals.size(); ++c) {
                        matrixData.emplace_back(vals[c]);
                    }
                }
                testCase.emplace_back(dimensions[0], dimensions[1], std::move(matrixData));
            }

            RandomMatrices.push_back({ testCase[0], testCase[1], testCase[2] });
        }
    }

};


TEST_F(MatrixTest, IDxSquareMatricesMultiplication)
{
    EXPECT_TRUE(MatrixTest::SquareMatrices.size() > 0)
        << "No test data for square matrices";
    size_t i = 0;
    for (auto const& [diagonal, matrices] : MatrixTest::SquareMatrices)
    {
        auto matrix = matrices[i++];
        i %= matrices.size();
        const Matrix<float> ID = Matrix<float>::ID(diagonal);
        EXPECT_TRUE(ID * matrix == matrix)
            << "ID * Square matrix with diagonal size: " << diagonal;
        EXPECT_TRUE(matrix * ID == matrix)
            << "Square * ID matrix with diagonal size: " << diagonal;
    }
}

TEST_F(MatrixTest, SquareMatricesMultiplication)
{
    EXPECT_TRUE(MatrixTest::SquareMatrices.size() > 0)
        << "No test data for square matrices";
    for (auto const& [diagonal, matrices] : MatrixTest::SquareMatrices)
    {
        auto A = matrices[0];
        auto B = matrices[1];
        auto C = matrices[2];
        EXPECT_TRUE(A*B == C);
    }
}

TEST_F(MatrixTest, IDxRandomMatricesMultiplication)
{
    EXPECT_TRUE(MatrixTest::RandomMatrices.size() > 0)
        << "No test data for matrices with random sizes";
    size_t i = 0;
    for (const auto& matrices : MatrixTest::RandomMatrices)
    {
        auto matrix = matrices[i++];
        i %= matrices.size();

        EXPECT_TRUE(Matrix<float>::ID(matrix.GetHeight()) * matrix == matrix);
        //EXPECT_TRUE(matrix * Matrix<float>::ID(matrix.GetWidth()) == matrix);
    }
}

TEST_F(MatrixTest, RandomMatricesMultiplication)
{
    EXPECT_TRUE(MatrixTest::RandomMatrices.size() > 0)
        << "No test data for matrices with random sizes";
    for (const auto& matrices : MatrixTest::RandomMatrices)
    {
        auto A = matrices[0];
        auto B = matrices[1];
        auto C = matrices[2];

        EXPECT_TRUE(A*B == C) << A  << '\n' << B << '\n' << C << std::endl;;
    }
}
