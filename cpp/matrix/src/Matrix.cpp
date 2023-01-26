#include "Matrix.hpp"
#include "Math.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>


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
{ return _mat._data[_mat.getDataIdx(_r, c)]; }


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
{ return _mat._data[_mat.getDataIdx(_r, c)]; }


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
Matrix<T>::Random(
    size_t rows,
    size_t columns,
    std::function<T()> generatorFunc,
    Matrix::Ordering ordering)
{ // static function
    std::vector<T> data;
    data.reserve(rows * columns);
    for (size_t i = 0; i < rows*columns; ++i) {
        data.push_back(generatorFunc());
    }
    return Matrix(rows, columns, std::move(data), ordering);
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, Matrix::Ordering ordering)
    : _rows(rows)
    , _columns(columns)
    , _ordering(ordering)
    , _data(_rows * _columns)
{
    //
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, const std::vector<T>&& data, Matrix::Ordering ordering)
    : _rows(rows)
    , _columns(columns)
    , _ordering(ordering)
    , _data(data)
{
    //
}

template<class T>
Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>> twoDimList, Matrix::Ordering ordering)
    : _rows(twoDimList.size())
    , _columns(twoDimList.begin()->size())
    , _ordering(ordering)
    , _data(0)
{
    // NOTE: Amount of rows is always > 0 if this ctor is called
    if (_columns == 0) {
        throw::std::invalid_argument("Matrix row must contain at least one element");
    }

    _data.reserve(_rows * _columns);

    switch (ordering)
    {
        case Ordering::RowMajor:
            for (const auto& r : twoDimList)
            {
                if (r.size() != _columns) {
                    throw std::invalid_argument("All rows in matrix must have the same amount of elements.");
                }

                _data.insert(_data.end(), r.begin(), r.end());
            }
            break;
        case Ordering::ColumnMajor:
            for (size_t listCol = 0; listCol < _columns; ++listCol)
            {
                for (size_t listRow = 0; listRow < _rows; ++listRow)
                {
                    if ((twoDimList.begin() + listRow)->size() != _columns) {
                        throw::std::invalid_argument("All rows in matrix must have the same amount of elements.");
                    }
                    _data.push_back(*( (twoDimList.begin() + listRow)->begin() + listCol ));
                }
            }
            break;
    }
}

template<class T> size_t
Matrix<T>::GetWidth() const { return _columns; }

template<class T> size_t
Matrix<T>::GetHeight() const { return _rows; }

template<class T> typename Matrix<T>::Ordering
Matrix<T>::GetOrdering() const { return _ordering; }

template<class T> typename Matrix<T>::Reference
Matrix<T>::operator[](size_t r) { return Matrix::Reference(*this, r); }

template<class T> const typename Matrix<T>::ConstReference
Matrix<T>::operator[](size_t r) const { return Matrix::ConstReference(*this, r); }

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

template<class T> Matrix<T>
Matrix<T>::operator+(const Matrix<T>& rhs) const{
    if (this->GetHeight() != rhs.GetHeight() ||
        this->GetWidth() != rhs.GetWidth())
    {
        throw std::invalid_argument("Mismatching matrix dimensions for addition");
    }

    assert(this->_data.size() == rhs._data.size());

    std::vector<T> sum;
    sum.reserve(this->_data.size());

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        std::transform(
            this->_data.begin(), this->_data.end(),
            rhs._data.begin(), std::back_inserter(sum),
            std::plus<T>()
        );
        return Matrix(this->GetHeight(), this->GetWidth(), std::move(sum), this->GetOrdering());
    }
    else
    {
        for (size_t r = 0; r < this->GetHeight(); ++r) {
            for (size_t c = 0; c < this->GetWidth(); ++c) {
                sum.push_back((*this)[r][c] + rhs[r][c]);
            }
        }
        return Matrix(this->GetHeight(), this->GetWidth(), std::move(sum), Ordering::RowMajor);
    }
}

template<class T> Matrix<T>
Matrix<T>::operator-(const Matrix<T>& rhs) const
{
    if (this->GetHeight() != rhs.GetHeight() ||
        this->GetWidth() != rhs.GetWidth())
    {
        throw std::invalid_argument("Mismatching matrix dimensions for subtraction");
    }

    assert(this->_data.size() == rhs._data.size());

    std::vector<T> diff;
    diff.reserve(this->_data.size());

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        std::transform(
            this->_data.begin(), this->_data.end(),
            rhs._data.begin(), std::back_inserter(diff),
            std::minus<T>()
        );
        return Matrix(this->GetHeight(), this->GetWidth(), std::move(diff), this->GetOrdering());
    }
    else
    {
        for (size_t r = 0; r < this->GetHeight(); ++r) {
            for (size_t c = 0; c < this->GetWidth(); ++c) {
                diff.push_back((*this)[r][c] - rhs[r][c]);
            }
        }
        return Matrix(this->GetHeight(), this->GetWidth(), std::move(diff), Ordering::RowMajor);
    }
}

template<class T> bool
Matrix<T>::operator==(const Matrix<T>& rhs) const
{
    if (this->GetHeight()  != rhs.GetHeight() ||
        this->GetWidth()   != rhs.GetWidth())
    {
        return false;
    }

    assert(this->_data.size() == rhs._data.size());

    constexpr T realTEpsilonFactor = static_cast<T>(3);
    // Use a wider than default factor for comparing values of RealType. This is
    // required for the tests, that compare matrices with float values, read from a
    // textual file that has been computed and saved by a python script.
    // TODO: Refactor Comparator so that the epsilon factor can be set from within the tests.

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        using namespace std::placeholders;
        return std::equal(
            this->_data.begin(), this->_data.end(),
            rhs._data.begin(),   rhs._data.end(),
            std::bind(&Math::AreEqual<T>, _1, _2, realTEpsilonFactor)
        );
    }

    for (size_t i = 0; i < this->GetHeight(); ++i)
    {
        for (size_t j = 0; j < this->GetWidth(); ++j)
        {
            if (!Math::AreEqual((*this)[i][j], rhs[i][j], realTEpsilonFactor)) {
                return false;
            }
        }
    }

    return true;
}


/****************************************
 * Matrix Private methods
 ****************************************/
template<class T> size_t
Matrix<T>::getDataIdx(size_t row, size_t col) const
{
    // TODO: Move checks away from here, here we trust the user of this method does not index out of bounds..
    if (row >= GetHeight()) {
        throw std::out_of_range("Row index: " + std::to_string(row) + " out of bounds.");
    }
    if (col >= GetWidth()) {
        throw std::out_of_range("Column index: " + std::to_string(col) + " out of bounds.");
    }

    switch (GetOrdering())
    {
        case Ordering::RowMajor:    return row * GetWidth() + col;
        case Ordering::ColumnMajor: return row + col * GetHeight();
    }

    assert(false);
    __builtin_unreachable();
}


/****************************************
 * Friend functions
 ****************************************/
template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat)
{
    constexpr size_t precision = 6;
    constexpr size_t fieldWidth = precision + 3; // Minimum reserved width for one entry.
                                                 // Fractional part + 2 for the integral part and one for the dot.
    std::ios_base::fmtflags out_flags(out.flags());

    auto printBar = [&out, precision](size_t cols) {
        out << "|-";
        for (size_t i = 0; i < cols * (fieldWidth + 1); ++i) {
            // Add one char for the space between every row value
            out << '-';
        }
        out << "|";
    };

    out << std::fixed << std::setprecision(precision);

    switch (mat.GetOrdering())
    {
        case Matrix<T>::Ordering::RowMajor:
            out << "| RowMajor Matrix of height X width: " << mat.GetHeight() << 'X' << mat.GetWidth() << " |\n";
            break;

        case Matrix<T>::Ordering::ColumnMajor:
            out << "| ColMajor Matrix of height X width: " << mat.GetHeight() << 'X' << mat.GetWidth() << " |\n";
            break;
    }

#ifndef NDEBUG
    out << "| RAW:";

    if (mat.GetHeight() * mat.GetWidth() <= 40)
    {
        out << " [ ";
        for (const auto& e : mat._data) std::cout << e << ", ";
        out << "] |\n";
    }
    else
    {
        out << " [ size > 50 => raw data not printed ] |\n";
    }
#endif

    printBar(mat.GetWidth());
    out << '\n';

    if (mat.GetHeight() > 0 && mat.GetWidth() > 0) {
        out << "| ";
    }

    for (size_t r = 0; r < mat.GetHeight(); ++r)
    {
        for (size_t c = 0; c < mat.GetWidth(); ++c)
        {
            out << std::setw(precision + 3) << mat[r][c];
            if ((c+1) != mat.GetWidth()) {
                out << ' ';
            } else {
                if ((r+1) != mat.GetHeight()) {
                    out << " |\n| ";
                } else {
                    out << " |\n";
                }
            }
        }
    }

    printBar(mat.GetWidth());

    out.flags(out_flags);
    return out;
}

template class Matrix<int>;
template class Matrix<size_t>;
template class Matrix<float>;
template class Matrix<double>;

template typename std::ostream& operator<<(std::ostream& out, const Matrix<int>& mat);
template typename std::ostream& operator<<(std::ostream& out, const Matrix<size_t>& mat);
template typename std::ostream& operator<<(std::ostream& out, const Matrix<float>& mat);
template typename std::ostream& operator<<(std::ostream& out, const Matrix<double>& mat);
