#include <iostream>
#include <sstream>
#include <vector>

#if __cplusplus > 201703L
#   include <bit>
#else
#   ifdef _MSC_VER
#       include <intrin.h>
#       define __builtin_popcountll __popcnt64
#   endif
#endif

#define NDEBUG
#include <cassert>

class BitArray
{
private:
    struct Indx
    {
        size_t arrIndx;
        size_t bitsIndx;
    };

    struct Reference
    {
        std::vector<uint64_t> & _arr;
        size_t                  _arrI;
        size_t                  _bitI;

        Reference(BitArray & bitArr, Indx idx)
            : _arr(bitArr._bitsArr)
            , _arrI(idx.arrIndx)
            , _bitI(idx.bitsIndx) { /**/ }

        Reference & operator=(bool v)
        {
            if (v) {
                _arr[_arrI] |= (1ULL << _bitI);
            } else {
                _arr[_arrI] &= ~(1ULL << _bitI);
            }
            return *this;
        }

        operator bool() {
            return _arr[_arrI] & (1ULL << _bitI);
        }
    };

    const size_t            _bitsWidth;
    size_t                  _length;
    std::vector<uint64_t>   _bitsArr;

public:
    BitArray(size_t size)
        : _bitsWidth(sizeof(uint64_t) * 8)
        , _length((assert(size > 0), (1 + (size-1)/_bitsWidth)))
        , _bitsArr(_length, 0)
    {
        //
    }

    // Returns the amount of set bits in this BitArray
    size_t count(void) const
    {
        size_t setBits = 0;
        for (size_t i = 0; i < _length; ++i) {
#if __cplusplus > 201703L
            setBits += static_cast<size_t>(std::popcount(_bitsArr[i]));
#else
//            Use the following (non tested) assembly code in case your compiler doesn't
//            support C++20 and doesn't provide the nonstandard function
//            __builtin_popcountll
/*
 *            uint64_t count;
 *            asm("popcnt %1,%0" : "=r"(count) : "rm"(_bitsArr[i]) : "cc");
 *            setBits += count;
 */
            setBits += static_cast<size_t>(__builtin_popcountll(_bitsArr[i]));
#endif
            // Slow version of the same.
            //for (size_t j = 0; j < _bitsWidth; ++j) {
            //    if (_bitsArr[i] & (1ULL << j)) {
            //        ++setBits;
            //    }
            //}
        }
        return setBits;
    }

    size_t size(void) const
    {
        return _length * _bitsWidth;
    }

    Reference operator[](size_t i)
    {
        const Indx idx = getIndxs(i);
        assert(i < size());
        assert(idx.arrIndx < _length);
        assert(idx.bitsIndx < _bitsWidth);
        return Reference(*this, idx);
    }

    void operator|=(const BitArray & other)
    {
        for (size_t i = 0; i < std::min(_length, other._length); ++i) {
            _bitsArr[i] |= other._bitsArr[i];
        }
    }

    friend std::ostream & operator<<(std::ostream & out, const BitArray & arr);

private:
    Indx getIndxs(size_t i) const
    {
        return { (_length-1) - (i/_bitsWidth), i%_bitsWidth };
    }

    void populateStrBuff(std::stringstream & buff) const
    {
        for (size_t i = 0; i < _length; ++i)
        {
            uint64_t n = _bitsArr[i];
            uint64_t b = _bitsWidth;
            while (b)
            {
                buff << ( (n & (1ULL << --b)) ? '1' : '0');
                if (b % 4 == 0) buff << ' ';
            }
        }
        buff << '\n';
    }
};

std::ostream & operator<<(std::ostream & out, const BitArray & arr)
{
    std::stringstream buff;
    arr.populateStrBuff(buff);
    out << buff.str();
    return out;
}

int main(void)
{
    BitArray bta(5);
    bta[0] = true;
    bta[2] = true;

    for (size_t i = 0; i < 5; ++i)
        std::cout << i << ": " << bta[i] << std::endl;
    std::cout << "-----" << std::endl;

    BitArray btb(4);
    btb[1] = true;

    for (size_t i = 0; i < 4; ++i)
        std::cout << i << ": " << btb[i] << std::endl;
    std::cout << "-----" << std::endl;

    bta |= btb;

    for (size_t i = 0; i < bta.size(); ++i)
        std::cout << i << ": " << bta[i] << std::endl;
    std::cout << "The constructed size of the array is always i*64 for some i >= 1" << std::endl;
}

