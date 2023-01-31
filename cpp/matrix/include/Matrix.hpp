#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "ThreadPool.hpp"

#include <array>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <vector>


template<class T>
class Matrix
{
public:

    enum class Ordering {
        // |1, 2, 3|
        // |4, 5, 6|
        // |7, 8, 9|
        RowMajor,   // row * nCols + col - 3x3 => 1 2 3 4 5 6 7 8 9
        ColumnMajor // row + col * nRows - 3x3 => 1 4 7 2 5 8 3 6 9
    };

    struct Reference
    {
        Reference(Matrix<T>& mat, size_t row);

        T& operator[](size_t col);

        Matrix<T>& _mat;
        const size_t _row;
    };

    struct ConstReference
    {
        ConstReference(const Matrix<T>& mat, const size_t row);

        const T& operator[](const size_t c) const;

        const Matrix<T>& _mat;
        const size_t _row;
    };


    // Factories for different types of precomputed matrices

    /// @brief Returns a identity matrix with dimensions of size X size
    /// @param size The width and height of the matrix
    /// @return Identity matrix
    static Matrix ID(size_t size);

    /// @brief Returns a matrix where each element is initialized to the value returned by the provided generator.
    /// @param rows Height of the matrix.
    /// @param columns Width of the matrix.
    /// @param generatorFunc A generator function that is called for every element in the matrix.
    /// @param ordering How the matrix should be saved in memory. Defaults to RowMajor ordering.
    /// @return The generated matrix.
    static Matrix Random(
        size_t rows,
        size_t columns,
        std::function<T()> generatorFunc,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    /// @brief Returns a matrix with where each element is set to the value returned by the provided generator.
    ///        Initializes the matrix values in concurrent fashion utilizing the threads provided by thread pool.
    /// @param rows Height of the matrix.
    /// @param columns Width of the matrix.
    /// @param generatorFunc A generator function that is called for every element in the matrix.
    /// @param threadPoolPtr A threadpool object holding worker threads.
    /// @param ordering How the matrix should be saved in memory. Defaults to RowMajor ordering.
    /// @return The generated matrix.
    static Matrix Random(
        size_t rows,
        size_t columns,
        std::function<T()> generatorFunc,
        const ThreadPool& threadPoolPtr,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    // Constructors

    Matrix() = delete;
    Matrix(Matrix&& other) = delete; // move ctor
    Matrix& operator=(const Matrix& other) = delete; // assignment ctor

    Matrix(const Matrix& other); // copy ctor

    /// @brief Sets all elements to default value of zero.
    Matrix(
        size_t rows,
        size_t columns,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    /// @brief Initializes a matrix with the data copied from the vector.
    /// @param rows Height of the matrix.
    /// @param columns Width of the matrix.
    /// @param data The values to initialize the matrix with. data.size() must equal rows * columns.
    /// @param ordering The ordering of the values in the data vector. The initialized matrix saves the values in the same order.
    Matrix(
        size_t rows,
        size_t columns,
        const std::vector<T>& data,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    /// @brief Initializes a new matrix by taking ownership of the unique_ptr data argument.
    /// @param data A unique ptr holding the data the matrix should be initialized with.
    Matrix(
        size_t rows,
        size_t columns,
        std::unique_ptr<T[]>&& data,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    /// @brief Initializes a new matrix object with the data in the 2-dimensional initializer list.
    /// @param twoDimList 2d array holding the values of the matrix.
    /// @param ordering How the matrix should be saved in memory. Defaults to RowMajor ordering.
    Matrix(
        const std::initializer_list<std::initializer_list<T>> twoDimList,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    size_t GetWidth()  const;
    size_t GetHeight() const;
    Matrix::Ordering GetOrdering() const;

    Matrix::Reference operator[](size_t row);
    const Matrix::ConstReference operator[](size_t row) const;

    /// @brief Multiplicates this (lhs) matrix with the rhs matrix and returns a new matrix holding the results.
    /// @return The computed matrix.
    Matrix operator*(const Matrix<T>& rhs) const;

    /// @brief Adds the rhs matrix to this (lhs) matrix and returns a new matrix holding the results.
    /// @return The computed matrix.
    Matrix operator+(const Matrix<T>& rhs) const;

    /// @brief Subtracts the rhs matrix from this (lhs) matrix and returns a new matrix holding the results.
    /// @return The computed matrix.
    Matrix operator-(const Matrix<T>& rhs) const;

    /// @brief Compares this matrix with the rhs matrix.
    /// @return true, if all elements are equal, false otherwise.
    bool operator==(const Matrix<T>& rhs)  const;

    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Matrix<U>& mat);

private:
    /// @brief Returns the index in the data array for the requested marix element.
    ///        This index differs for matrices stored in column vs. row major order.
    /// @param row The row of the requested element in the matrix.
    /// @param col The column of the requested element in the matrix.
    /// @return the computed index for the element.
    size_t getDataIdx(size_t row, size_t col) const;

    size_t getDataIdx(size_t row, size_t col, Matrix::Ordering ordering) const;
    size_t getLength() const;


private:
  const size_t         _rows;
  const size_t         _columns;
  Ordering             _ordering;
  std::unique_ptr<T[]> _data;

};


template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat);


#endif // MATRIX_HPP
