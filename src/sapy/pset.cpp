#include "sapy/pset.h"


namespace sapy {

size_t PSet::HashFunc::operator()(const PAnyWrapper &obj) const {
    return obj.hash();
}

size_t PSet::hash() const {
    size_t result = 0;
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        result ^= it->hash();
    }
    return result;
}



PString PSet::toString() const {
    if(container_.empty()){
        return "set()";
    }
    PString result = "{";
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        result += (it->isString()?  "\"" + it->toString() + "\"" : it->toString());
        if (std::next(it) != container_.end()) {
            result += ", ";
        }
    }
    result += "}";
    return result;
}

void PSet::_print(std::ostream& os) const {
    os << toString();
}

void PSet::add(const PAnyWrapper& elem) {
    container_.insert(elem);
}

PSet PSet::copy() const {
    PSet result;
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        result.add(*it);
    }
    return result;
}

PSet PSet::difference(const PSet& other) const {
    PSet result;
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        if (!other.contains(*it)) {
            result.add(*it);
        }
    }
    return result;
}

void PSet::difference_update(const PSet& other) {
    for (auto it = container_.begin(); it != container_.end(); ) {
        if (other.contains(*it)) {
            it = container_.erase(it);
        } else {
            ++it;
        }
    }
}

void PSet::discard(const PAnyWrapper& elem) {
    container_.erase(elem);
}

PSet PSet::intersection(const PSet& other) const {
    PSet result;
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        if (other.contains(*it)) {
            result.add(*it);
        }
    }
    return result;
}

void PSet::intersection_update(const PSet& other) {
    for (auto it = container_.begin(); it != container_.end(); ) {
        if (!other.contains(*it)) {
            it = container_.erase(it);
        } else {
            ++it;
        }
    }
}

bool PSet::isdisjoint(const PSet& other) const {
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        if (other.contains(*it)) {
            return false;
        }
    }
    return true;
}


bool PSet::issubset(const PSet& other) const {
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        if (!other.contains(*it)) {
            return false;
        }
    }
    return true;
}

bool PSet::issuperset(const PSet& other) const {
    return other.issubset(*this);
}

PSet PSet::union_(const PSet& other) const {
    PSet result = copy();
    for (auto it = other.container_.begin(); it != other.container_.end(); ++it) {
        result.add(*it);
    }
    return result;
}

PAnyWrapper PSet::pop() {
    if (container_.empty()) {
        throw std::out_of_range("pop from an empty set");
    }
    auto it = container_.begin();
    PAnyWrapper result = *it;
    container_.erase(it);
    return result;
}

PSet PSet::symmetric_difference(const PSet& other) const {
    auto s1 = difference(other);
    auto s2 = other.difference(*this);
    return s1.union_(s2);
}

void PSet::symmetric_difference_update(const PSet& other) {
    auto s1 = difference(other);
    auto s2 = other.difference(*this);
    container_ = s1.union_(s2).container_;
}

void PSet::update(const PSet& other) {
    for (auto it = other.container_.begin(); it != other.container_.end(); ++it) {
        add(*it);
    }
}

PSet PSet::operator-(const PSet& other) const {
    return difference(other);
}

PSet PSet::operator-(const PAnyWrapper& other) const {
    PSet result = copy();
    result.discard(other);
    return result;
}

bool PSet::operator==(const PSet& other) const {
    if (container_.size() != other.container_.size()) {
        return false;
    }
    for (auto it = container_.begin(); it != container_.end(); ++it) {
        if (!other.contains(*it)) {
            return false;
        }
    }
    return true;
}

}