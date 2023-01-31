#include "Matrix.hpp"
#include "Math.hpp"
#include "ThreadPool.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>


/****************************************
 * Matrix::Reference implemenentation
 ****************************************/
template<class T>
Matrix<T>::Reference::Reference(Matrix<T>& mat, size_t row)
    : _mat(mat)
    , _row(row)
{
    //
}

template<class T> T&
Matrix<T>::Reference::operator[](size_t col)
{
    if (col < _mat.GetWidth()) {
        return _mat._data[_mat.getDataIdx(_row, col)];
    }
    throw std::out_of_range("Column index: " + std::to_string(col) + " out of bounds.");
}


/****************************************
 * Matrix::ConstReference implemenentation
 ****************************************/
template<class T>
Matrix<T>::ConstReference::ConstReference(const Matrix<T>& mat, const size_t row)
    : _mat(mat)
    , _row(row)
{
    //
}

template<class T> const T&
Matrix<T>::ConstReference::operator[](const size_t col) const
{
    if (col < _mat.GetWidth()) {
        return _mat._data[_mat.getDataIdx(_row, col)];
    }
    throw std::out_of_range("Column index: " + std::to_string(col) + " out of bounds.");
}


/****************************************
 * Matrix implemenetation
 ****************************************/

template<class T> Matrix<T>
Matrix<T>::ID(size_t size)
{ // static function
    std::unique_ptr<T[]> data = std::make_unique<T[]>(size * size);

    for (size_t r = 0; r < size; ++r) {
        for (size_t c = 0; c < size; ++c) {
            data[r*size + c] = static_cast<T>(r == c ? 1 : 0);
        }
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
    size_t length = rows*columns;

    std::unique_ptr<T[]> data = std::make_unique<T[]>(length);

    if (ThreadPool::IsStarted())
    {
        size_t lengthSlice = length / ThreadPool::GetThreadsCount();
        if (lengthSlice < MIN_OPERATIONS_PER_THREAD) {
            lengthSlice = MIN_OPERATIONS_PER_THREAD;
        }
        size_t i = 0;

        std::vector<std::future<bool>> threadResults;

        // Function for the worker threads to run.
        const auto fillArrSegment = [&data, &generatorFunc](size_t start, size_t end)
        {
            while (start < end) {
                data[start++] = generatorFunc();
            }

            return true;
        };

        for (; lengthSlice > 0 && (i+lengthSlice) <= length; i += lengthSlice) {
            // Start threads, each thread fills a disjoint range of the data array.
            threadResults.push_back(ThreadPool::QueueTask(std::bind(fillArrSegment, i, i+lengthSlice)));
        }

        while (i < length) {
            // Main thread fills the remainder range, with length < worker threads range.
            data[i++] = generatorFunc();
        }

        for (auto& r : threadResults) {
            // Wait for all threads to complete before continuing.
            r.get();
        }
    }
    else
    {
        for (size_t i = 0; i < rows*columns; ++i) {
            data[i] = generatorFunc();
        }
    }

    return Matrix(rows, columns, std::move(data), ordering);
}


template<class T>
Matrix<T>::Matrix(const Matrix& other)
    : _rows(other._rows)
    , _columns(other._columns)
    , _ordering(other._ordering)
    , _data(std::make_unique<T[]>(other.getLength()))
{
    for (size_t i = 0; i < other.getLength(); ++i) {
        _data[i] = other._data[i];
    }
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, Matrix::Ordering ordering)
    : _rows(rows)
    , _columns(columns)
    , _ordering(ordering)
    , _data(std::make_unique<T[]>(_rows * _columns))
{
    assert(_rows * _columns == getLength());
    for (size_t i = 0; i < getLength(); ++i) {
        _data[i] = static_cast<T>(0);
    }
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, const std::vector<T>& data, Matrix::Ordering ordering)
    : _rows(rows)
    , _columns(columns)
    , _ordering(ordering)
    , _data(std::make_unique<T[]>(data.size()))
{
    assert(getLength() == data.size());
    for (size_t i = 0; i < getLength(); ++i) {
        _data[i] = data[i];
    }
}

template<class T>
Matrix<T>::Matrix(size_t rows, size_t columns, std::unique_ptr<T[]>&& data, Matrix::Ordering ordering)
    : _rows(rows)
    , _columns(columns)
    , _ordering(ordering)
    , _data(std::move(data))
{
    //
}

template<class T>
Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>> twoDimList, Matrix::Ordering ordering)
    : _rows(twoDimList.size())
    , _columns(twoDimList.begin()->size())
    , _ordering(ordering)
    , _data(std::make_unique<T[]>(_rows * _columns))
{
    // NOTE: Amount of rows is always > 0 if this ctor is called
    if (_columns == 0) {
        throw::std::invalid_argument("Matrix row(s) must contain at least one element");
    }

    size_t r = 0;
    for (const auto& rowData : twoDimList)
    {
        if (rowData.size() != _columns) {
            throw std::invalid_argument("All rows in matrix must have the same amount of elements.");
        }
        size_t c = 0;
        for (T v : rowData) {
            _data[getDataIdx(r, c++)] = v;
        }
        ++r;
    }
}


template<class T> size_t
Matrix<T>::GetWidth() const { return _columns; }

template<class T> size_t
Matrix<T>::GetHeight() const { return _rows; }

template<class T> typename Matrix<T>::Ordering
Matrix<T>::GetOrdering() const { return _ordering; }

template<class T> typename Matrix<T>::Reference
Matrix<T>::operator[](size_t row) {
    if (row >= GetHeight()) {
        throw std::out_of_range("Row index: " + std::to_string(row) + " out of bounds.");
    }

    return Matrix::Reference(*this, row);
}

template<class T> const typename Matrix<T>::ConstReference
Matrix<T>::operator[](size_t row) const {
    if (row >= GetHeight()) {
        throw std::out_of_range("Row index: " + std::to_string(row) + " out of bounds.");
    }

    return Matrix::ConstReference(*this, row);
}

template<class T> Matrix<T>
Matrix<T>::operator*(const Matrix<T>& rhs) const
{

    if (this->GetWidth() != rhs.GetHeight()) {
        throw std::invalid_argument("Mismatching matrix dimensions for multiplication.");
    }

    const size_t height = this->GetHeight();
    const size_t width  = rhs.GetWidth();

    std::unique_ptr<T[]> data = std::make_unique<T[]>(height * width);

    if (ThreadPool::IsStarted())
    {
        size_t rowSlice = height / ThreadPool::GetThreadsCount();
        if ((rowSlice * this->GetWidth()) < MIN_OPERATIONS_PER_THREAD) {
            rowSlice = MIN_OPERATIONS_PER_THREAD / this->GetWidth();
        }

        size_t r = 0;

        std::vector<std::future<bool>> threadResults;

        const auto computeRows = [this, &data, &width, &height, &rhs](size_t row, size_t endRow)
        {
            while (row < endRow)
            {
                for (size_t c = 0; c < width; ++c)
                {
                    T sum = static_cast<T>(0);
                    for (size_t i = 0; i < this->GetWidth(); ++i)
                    {
                        sum += this->_data[this->getDataIdx(row, i)] * rhs._data[rhs.getDataIdx(i, c)];
                    }
                    data[row * width + c] = sum;
                }
                ++row;
            }

            return true;
        };

        for (; rowSlice > 0 && (r+rowSlice) <= height; r += rowSlice) {
            threadResults.push_back(ThreadPool::QueueTask(std::bind(computeRows, r, r+rowSlice)));
        }

        computeRows(r, height); // Compute possible remaining rows.

        for (auto& res : threadResults) {
            res.get();
        }
    }
    else
    {
        for (size_t r = 0; r < height; ++r)
        {
            for (size_t c = 0; c < width; ++c)
            {
                T sum = static_cast<T>(0);
                for (size_t i = 0; i < this->GetWidth(); ++i)
                {
                    sum += this->_data[this->getDataIdx(r, i)] * rhs._data[rhs.getDataIdx(i, c)];
                }
                data[r * width + c] = sum;
            }
        }
    }

    return Matrix(height, width, std::move(data), Matrix<T>::Ordering::RowMajor);
}

template<class T> Matrix<T>
Matrix<T>::operator+(const Matrix<T>& rhs) const{
    if (this->GetHeight() != rhs.GetHeight() ||
        this->GetWidth() != rhs.GetWidth())
    {
        throw std::invalid_argument("Mismatching matrix dimensions for addition");
    }

    assert(this->getLength() == rhs.getLength());

    std::unique_ptr<T[]> data = std::make_unique<T[]>(this->getLength());

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        for (size_t i = 0; i < this->getLength(); ++i) {
            data[i] = this->_data[i] + rhs._data[i];
        }
    }
    else
    {
        for (size_t r = 0; r < this->GetHeight(); ++r) {
            for (size_t c = 0; c < this->GetWidth(); ++c) {
                data[getDataIdx(r, c)] =
                    this->_data[getDataIdx(r, c)] + rhs._data[rhs.getDataIdx(r, c)];
            }
        }
    }

    return Matrix(this->GetHeight(), this->GetWidth(), std::move(data), this->GetOrdering());
}

template<class T> Matrix<T>
Matrix<T>::operator-(const Matrix<T>& rhs) const
{
    if (this->GetHeight() != rhs.GetHeight() ||
        this->GetWidth() != rhs.GetWidth())
    {
        throw std::invalid_argument("Mismatching matrix dimensions for subtraction");
    }

    assert(this->getLength() == rhs.getLength());

    std::unique_ptr<T[]> data = std::make_unique<T[]>(this->getLength());

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        for (size_t i = 0; i < this->getLength(); ++i) {
            data[i] = this->_data[i] - rhs._data[i];
        }
    }
    else
    {
        for (size_t r = 0; r < this->GetHeight(); ++r) {
            for (size_t c = 0; c < this->GetWidth(); ++c) {
                data[getDataIdx(r, c)] =
                    this->_data[getDataIdx(r, c)] - rhs._data[rhs.getDataIdx(r, c)];
            }
        }
    }

    return Matrix(this->GetHeight(), this->GetWidth(), std::move(data), this->GetOrdering());
}

template<class T> bool
Matrix<T>::operator==(const Matrix<T>& rhs) const
{
    if (this->GetHeight()  != rhs.GetHeight() ||
        this->GetWidth()   != rhs.GetWidth())
    {
        return false;
    }

    assert(this->getLength() == rhs.getLength());

    constexpr T realTEpsilonFactor = static_cast<T>(3);
    // Use a wider than default factor for comparing values of RealType. This is
    // required for the tests, that compare matrices with float values, read from a
    // textual file that has been computed and saved by a python script.
    // TODO: Refactor Comparator so that the epsilon factor can be set from within the tests.

    if (this->GetOrdering() == rhs.GetOrdering())
    {
        for (size_t i = 0; i < this->getLength(); ++i) {
            if (!Math::AreEqual(this->_data[i], rhs._data[i], realTEpsilonFactor)) {
                return false;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < this->GetHeight(); ++i) {
            for (size_t j = 0; j < this->GetWidth(); ++j) {
                if (!Math::AreEqual(this->_data[getDataIdx(i, j)], rhs._data[rhs.getDataIdx(i, j)], realTEpsilonFactor)) {
                    return false;
                }
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
    return getDataIdx(row, col, GetOrdering());
}

template<class T> size_t
Matrix<T>::getDataIdx(size_t row, size_t col, Matrix<T>::Ordering ordering) const
{
    switch (ordering)
    {
        case Ordering::RowMajor:    return row * GetWidth() + col;
        case Ordering::ColumnMajor: return row + col * GetHeight();
    }

    assert(false);
    __builtin_unreachable();
}

template<class T> size_t
Matrix<T>::getLength() const
{ return GetHeight() * GetWidth(); }


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

    if (mat.getLength() <= 50)
    {
        out << " [ ";
        for (size_t i = 0; i < mat.getLength(); ++i) {
            std::cout << mat._data[i] << ", ";
        }
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
