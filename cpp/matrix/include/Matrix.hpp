#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <functional>
#include <iostream>
#include <initializer_list>
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
        Reference(Matrix<T>& mat, size_t r);

        T& operator[](size_t c);

        Matrix<T>& _mat;
        const size_t _r;
    };

    struct ConstReference
    {
        ConstReference(const Matrix<T>& mat, const size_t r);

        const T& operator[](const size_t c) const;

        const Matrix<T>& _mat;
        const size_t _r;
    };


    static Matrix ID(size_t size);

    static Matrix Random(
        size_t rows,
        size_t columns,
        std::function<T()> generatorFunc,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );

    Matrix() = delete;
    //Matrix(Matrix& other) = delete; // copy ctor, uses std::move ?
    //Matrix(const Matrix& other) = delete;

    Matrix(
        size_t rows,
        size_t columns,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );
    Matrix(
        size_t rows,
        size_t columns,
        const std::vector<T>&& data,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );
    Matrix(
        const std::initializer_list<std::initializer_list<T>> twoDimList,
        Matrix::Ordering ordering = Matrix::Ordering::RowMajor
    );
    //Matrix& operator=(const Matrix& other) = delete;

    size_t GetWidth()  const;
    size_t GetHeight() const;
    Matrix::Ordering GetOrdering() const;
    Matrix::Reference operator[](size_t r);
    const Matrix::ConstReference operator[](size_t r) const;

    Matrix operator*(const Matrix<T>& rhs) const;
    Matrix operator+(const Matrix<T>& rhs) const;
    Matrix operator-(const Matrix<T>& rhs) const;
    bool operator==(const Matrix<T>& rhs) const;

    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Matrix<U>& mat);

private:
    /// @brief Returns the index in the data array for the requested marix element.
    ///        This index differs for matrices stored in column vs. row major order.
    /// @param row The row of the requested element in the matrix.
    /// @param col The column of the requested element in the matrix.
    /// @return the computed index for the element.
    size_t getDataIdx(size_t row, size_t col) const;


private:
  const size_t   _rows;
  const size_t   _columns;
  Ordering       _ordering;
  std::vector<T> _data;

};


template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat);


#endif // MATRIX_HPP
