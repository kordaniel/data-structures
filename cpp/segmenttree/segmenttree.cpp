#include <iostream>
#include <sstream>
#include <vector>
#include <limits>

#define NDEBUG
#include <cassert>

size_t next_pow_of_2(size_t n)
{
    --n;
    for (size_t bits = 1; bits < sizeof(n) * 8; bits <<= 1)
        n |= n >> bits;

    return n == 0 ? 2 : n+1;
}

template<typename T>
class SegmentTree
{
private:
    struct Node
    {
        T s = 0;   // The sum in the Node
        T z = 0;   // The lazy propagation that is to be added
                   // to every element under this node
    };
    size_t            _size;
    std::vector<Node> _tree;

public:
    SegmentTree(const size_t size)
        : _size(next_pow_of_2(size))
        , _tree(2 * _size)
    {
        //
    }

    // Update the sum with the value of val in the range [a,b]
    void updateRange(size_t a, size_t b, const T val)
    {
        updateRange(a, b, 1, val, 0, this->_size - 1);
    }

    // Should not be called from outside this class.
    void updateRange(size_t a, size_t b, size_t i, const T val, size_t l, size_t r)
    { // segment to be updated         |Array idx| Value to be| Node covers Segment
      // [a, b]                        |         | added      | [l,r]
        assert(a < _size);
        assert(b < _size);
        if (a > b)
            return;
        if (a == l && b == r)
        {   // This node [l,r] covers exactly the whole segment [a,b]
            _tree[i].z += val;
            return;
        }

        const T overlap = static_cast<T>(std::min(b, r) - std::max(a, l)) + 1;
        const size_t d  = (l + r) / 2;

        _tree[i].s     += overlap * val;

        if (_tree[i].z != 0)
            descend_lazy_change(i, l, r);

        updateRange(a,                std::min(b, d), 2*i,   val, l,   d);
        updateRange(std::max(a, d+1), b,              2*i+1, val, d+1, r);
    }

    // Returns the total sum in the range [a,b]
    T getSum(size_t a, size_t b)
    {
        return getSum(a, b, 1, 0, this->_size - 1);
    }

    // Should not be called from outside this class.
    T getSum(size_t a, size_t b, size_t i, size_t l, size_t r)
    { // query segment         |Array idx| node covering segment
      //         left,    right|         |       l and r
        assert(a < _size);
        assert(b < _size);
        if (a > b || b < l || a > r)
            return 0;

        if (i >= _size) {
            _tree[i].s += _tree[i].z;
            _tree[i].z  = 0;
            return _tree[i].s;
        }

        if (l >= a && r <= b) {
            assert((r-l+1) >= 0);
            assert((r-l+1) <= std::numeric_limits<T>::max());
            return _tree[i].s + static_cast<T>(r-l+1) * _tree[i].z;
        }

        if (_tree[i].z != 0)
            descend_lazy_change(i, l, r);

        const size_t d = (l+r) / 2;
        return getSum(a,                std::min(b, d), 2*i,   l,   d) +
               getSum(std::max(a, d+1), b,              2*i+1, d+1, r);
    }

    T getElementAt(size_t i)
    {
        return getSum(i, i);
    }

    size_t size(void)
    {
        return _size;
    }

    template<typename Y>
    friend std::ostream & operator<<(std::ostream & out, const SegmentTree<Y> & tree);

private:
    void descend_lazy_change(const size_t i, const size_t l, const size_t r)
    {
        _tree[i].s     += static_cast<T>(r-l+1) * _tree[i].z;
        _tree[2*i].z   += _tree[i].z;
        _tree[2*i+1].z += _tree[i].z;
        _tree[i].z      = 0;
    }

    void constructPrintTree(std::stringstream & buff,
                            std::string prefix,
                            std::string childrenPrefix,
                            const size_t idx = 1) const
    {
        buff << prefix << '(' << idx << "): {" << _tree[idx].s << ", " <<_tree[idx].z << "}\n";
        if (idx >= _size)
            return;

        constructPrintTree(buff, childrenPrefix + "├── ", childrenPrefix + "|   ", 2 * idx);
        constructPrintTree(buff, childrenPrefix + "└── ", childrenPrefix + "    ", 2 * idx + 1);
    }

};

template<typename T>
std::ostream & operator<<(std::ostream & out, const SegmentTree<T> & t)
{
    std::stringstream buff;
    t.constructPrintTree(buff, "", "");
    out << buff.str();
    return out;
}

int main(void)
{
    SegmentTree<int> t(13); // Actual size will always be the next power of 2

    std::cout << "SegmentTree size: " << t.size() << std::endl;
    std::cout << t;
    t.updateRange(1, 3, 3);
    std::cout << t;
    std::cout << "SUM of segment 1-3: " << t.getSum(1,3) << std::endl;
    std::cout << "------------------------------" << std::endl;

    t.updateRange(2, 10, 2);
    std::cout << t;
    std::cout << "SUM of segment 1-10: " << t.getSum(1, 10) << std::endl;
    std::cout << "SUM of segment 2-10: " << t.getSum(2, 10) << std::endl;
    std::cout << "SUM of segment 2-3:  "  << t.getSum(2, 3) << std::endl;
    std::cout << "------------------------------" << std::endl;

    t.updateRange(2, 10, -5);
    std::cout << t;
    std::cout << "SUM of segment 1-1:  "  << t.getSum(1, 1) << std::endl;
    std::cout << "SUM of segment 2-3:  "  << t.getSum(2, 3) << std::endl;
    std::cout << "SUM of segment 4-10: " << t.getSum(4, 10) << std::endl;
    std::cout << "SUM of segment 1-10: " << t.getSum(1, 10) << std::endl;
    std::cout << "------------------------------" << std::endl;

    t.updateRange(0, 2, 1);
    std::cout << t;
    std::cout << "SUM of segment 0-0: " << t.getSum(0, 0) << std::endl;
    std::cout << "SUM of segment 0-1: " << t.getSum(0, 1) << std::endl;
    std::cout << "SUM of segment 0-2: " << t.getSum(0, 2) << std::endl;
    std::cout << "Total SUM in array: " << t.getSum(0, t.size()-1) << std::endl;
    std::cout << "------------------------------" << std::endl;

    return 0;
}
