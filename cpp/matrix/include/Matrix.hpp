#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <iostream>
#include <array>
#include <initializer_list>


template<class T>
class Matrix
{
public:

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
    static Matrix Random(size_t rows, size_t columns, T minInclusive, T maxInclusive);

    Matrix() = delete;
    //Matrix(Matrix& other) = delete; // copy ctor, uses std::move ?
    //Matrix(const Matrix& other) = delete;

    Matrix(size_t rows, size_t columns);
    Matrix(size_t rows, size_t columns, const std::vector<T>&& data);
    Matrix(std::initializer_list<std::initializer_list<T>> twoDimList);
    //Matrix& operator=(const Matrix& other) = delete;

    size_t GetWidth()  const;
    size_t GetHeight() const;
    Matrix::Reference operator[](size_t r);
    Matrix::ConstReference operator[](size_t r) const;

    Matrix operator*(const Matrix<T>& rhs) const;
    bool operator==(const Matrix<T>& rhs) const;

    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, const Matrix<U>& mat);

private:
  const size_t _rows;
  const size_t _columns;
  std::vector<T> _data;

};


template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat);


#endif // MATRIX_HPP
