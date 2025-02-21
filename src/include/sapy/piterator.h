#pragma once

#include "sapy/panywrapper.h"
#include "sapy/pcollection_interface.h"
#include <algorithm>
#include <iterator>

namespace sapy{
class PString;

template <typename Container>
class PIterator {
public:

    using IteratorType = typename Container::iterator;
    using ConstIteratorType = typename Container::const_iterator;
    using ValueType = typename Container::value_type;
    using ConstValueType = typename Container::const_value_type;

    PIterator(IteratorType iter) : _iter(iter) {}
    PIterator(ConstIteratorType iter) : _iter(iter) {}

    // Do not use reference here
    ValueType operator*() {
        return *_iter;
    }

    // Do not use reference here
    const ValueType operator*() const {
        return *_iter;
    }


    PIterator& operator++() {
        ++_iter;
        return *this;
    }

    PIterator operator++(int) {
        PIterator temp = *this;
        ++(*this);
        return temp;
    }

    PIterator operator+(size_t n) {
        return PIterator(_iter + n);
    }

    bool operator!=(const PIterator& other) const {
        return _iter != other._iter;
    }

    bool operator==(const PIterator& other) const {
        return _iter == other._iter;
    }

    ConstValueType* operator->() const { return &(*_iter); }
    // virtual PString toString() const override {
    //     return PString("PIterator");
    // }

private:
    // virtual void _print(std::ostream& os) const override {
    //     os << toString();
    // }

    ConstIteratorType _iter;  
};

}