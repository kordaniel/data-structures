#include "Matrix.hpp"
#include "Math.hpp"

#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <cassert>


/****************************************
 * Matrix::Reference implemenentation
 ****************************************/
template<class T>
Matrix<T>::Reference::Reference(Matrix<T>& mat, size_t r)
    : _mat(mat)
    , _r(r)
{
    //
}

template<class T> T&
Matrix<T>::Reference::operator[](size_t c)
{
    if (c >= _mat.GetWidth()) {
        throw std::out_of_range("Column index: " + std::to_string(c) + " out of bounds.");
    }

    return _mat._data[_r * _mat.GetWidth() + c];
}


/****************************************
 * Matrix::ConstReference implemenentation
 ****************************************/
template<class T>
Matrix<T>::ConstReference::ConstReference(const Matrix<T>& mat, const size_t r)
    : _mat(mat)
    , _r(r)
{
    //
}

template<class T> const T&
Matrix<T>::ConstReference::operator[](const size_t c) const
{
    if (c >= _mat.GetWidth()) {
        throw std::out_of_range("Column index: " + std::to_string(c) + " out of bounds.");
    }

    return _mat._data[_r * _mat.GetWidth() + c];
}


/****************************************
 * Matrix implemenetation
 ****************************************/

template<class T> Matrix<T>
Matrix<T>::ID(size_t size)
{ // static function
    std::vector<T> data(size * size);
    for (size_t i = 0; i < size; ++i) {
        data[i*size + i] = static_cast<T>(1);
    }
    return Matrix(size, size, std::move(data));
}

template<class T> Matrix<T>
Matrix<T>::Random(size_t rows, size_t columns, T minInclusive, T maxInclusive)
{ // static function
    std::vector<T> data;
    data.reserve(rows * columns);
    for (size_t i = 0; i < rows*columns; ++i)
    {
        if constexpr (std::is_same<T, float>() || std::is_same<T, double>())
        {
            data.push_back(Random::Real(minInclusive, maxInclusive));
        }
        else if constexpr (std::is_same<T, int>())
        {
            data.push_back(Random::Integer(minInclusive, maxInclusive));
        }
        else
        { // NOTE: If (only) the supported template types are defined at the last rows of this module, then
          //       the compiler should never branch here, so this is just a sanity check.
            static_assert(!sizeof(T*), "Matrix::Random called with unsupported template type");
        }
    }
    return Matrix(rows, columns, std::move(data));
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns)
    : _rows(rows)
    , _columns(columns)
    , _data(_rows * _columns)
{
    //
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, const std::vector<T>&& data)
    : _rows(rows)
    , _columns(columns)
    , _data(data)
{
    //
}

template<class T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> twoDimList)
    : _rows(twoDimList.size())
    , _columns(twoDimList.begin()->size())
    , _data(0)
{
    // NOTE: num of rows is always > 0 if this ctor is called
    if (_columns == 0) {
        throw::std::invalid_argument("Matrix row must contain at least one element");
    }

    _data.reserve(_rows * _columns);

    for (auto& r : twoDimList)
    {
        if (r.size() != _columns) {
            throw std::invalid_argument("All rows in matrix must have the same amount of elements");
        }

        _data.insert(_data.end(), r.begin(), r.end());
    }
}

template<class T> size_t
Matrix<T>::GetWidth() const { return _columns; }

template<class T> size_t
Matrix<T>::GetHeight() const { return _rows; }

template<class T> typename Matrix<T>::Reference
Matrix<T>::operator[](size_t r)
{
    if (r >= GetHeight()) {
        throw std::out_of_range("Row index: " + std::to_string(r) + " out of bounds.");
    }

    return Matrix::Reference(*this, r);
}

template<class T> typename Matrix<T>::ConstReference
Matrix<T>::operator[](size_t r) const
{
    if (r >= GetHeight()) {
        throw std::out_of_range("Row index: " + std::to_string(r) + " out of bounds.");
    }

    return Matrix::ConstReference(*this, r);
}

template<class T> Matrix<T>
Matrix<T>::operator*(const Matrix<T>& rhs) const
{
    if (this->GetWidth() != rhs.GetHeight()) {
        throw std::invalid_argument("Mismatching matrix dimensions for multiplication.");
    }

    Matrix<T> product(this->GetHeight(), rhs.GetWidth());

    for (size_t r = 0; r < this->GetHeight(); ++r)
    {
        for (size_t c = 0; c < rhs.GetWidth(); ++c)
        {
            T sum = static_cast<T>(0);
            for (size_t i = 0; i < this->GetWidth(); ++i)
            {
                sum += (*this)[r][i] * rhs[i][c];
            }
            product[r][c] = sum;
        }
    }

    return product;
}

template<class T> bool
Matrix<T>::operator==(const Matrix<T>& rhs) const
{
    if (this->GetHeight()  != rhs.GetHeight() ||
        this->GetWidth()   != rhs.GetWidth() ||
        this->_data.size() != rhs._data.size())
    {
        return false;
    }

    return std::equal(
        this->_data.begin(), this->_data.end(),
        rhs._data.begin(),   rhs._data.end(),
        Math::AreEqual<T>
    );
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat)
{
    constexpr size_t precision = 8;
    std::ios_base::fmtflags out_flags(out.flags());

    auto printBar = [&out, precision](size_t cols) {
        out << "|-";
        for (size_t i = 0; i < cols * (precision+3); ++i) {
            // precision = length of fractional part. In addition one char
            // for the dot, one for the space and one for the integral part
            out << '-';
        }
        out << "|";
    };

    out << std::fixed << std::setprecision(precision);

    out << "| Matrix of height X width: " << mat.GetHeight() << 'X' << mat.GetWidth() << " |\n";
    printBar(mat.GetWidth());

    out << '\n';

    if (mat.GetHeight() > 0 && mat.GetWidth() > 0) {
        out << "| ";
    }

    for (size_t i = 0; i < (mat.GetHeight() * mat.GetWidth()); ++i)
    {
        out << std::setw(10) << mat._data[i];
        if ((i+1) % mat.GetWidth() != 0) {
            out << ' ';
        } else if (i != (mat.GetHeight() * mat.GetWidth() - 1)  ) {
            out << " |\n| ";
        } else {
            out << " |\n";
        }
    }

    printBar(mat.GetWidth());

    out.flags(out_flags);
    return out;
}

template class Matrix<int>;
template class Matrix<float>;
template class Matrix<double>;

template typename std::ostream& operator<<(std::ostream& out, const Matrix<int>& mat);
template typename std::ostream& operator<<(std::ostream& out, const Matrix<float>& mat);
template typename std::ostream& operator<<(std::ostream& out, const Matrix<double>& mat);
