#pragma once 


#include "sapy/pcollection_interface.h"
#include "sapy/panywrapper.h"
#include "sapy/piterator.h"
#include "sapy/pobject.h"
#include "sapy/pstring.h"
#include <unordered_set>
#include <iostream>

namespace sapy {

class PSet : public PObject, public PCollectionInterface<PSet> {
private:
    friend class PCollectionInterface<PSet>;

    /*****************Implement PCollectionInterface methods*******************/

    template <typename... Args>
    inline void appendImpl(Args &&...args) {
        container_.insert(std::forward<Args>(args)...);
    }

    inline void clearImpl() {
        container_.clear();
    }

    inline size_t sizeImpl() const {
        return container_.size();
    }

    inline bool removeImpl(const PAnyWrapper &elem) {
        return container_.erase(elem);
    }

    inline bool containImpl(const PAnyWrapper &elem) const {
        return container_.find(elem) != container_.end();
    }



    struct HashFunc {
        size_t operator()(const PAnyWrapper &obj) const;
    };

public: 
    PSet() = default;
    PSet(const PSet& other) : container_(other.container_){};
    PSet(std::initializer_list<PAnyWrapper> init) : container_(init){};
    size_t hash() const override;
    PString toString() const override;
    void add(const PAnyWrapper& elem);
    PSet copy() const;
    PSet difference(const PSet& other) const;
    void difference_update(const PSet& other);
    void discard(const PAnyWrapper& elem);
    PSet intersection(const PSet& other) const;
    void intersection_update(const PSet& other);
    bool isdisjoint(const PSet& other) const;
    bool issubset(const PSet& other) const;
    bool issuperset(const PSet& other) const;
    PAnyWrapper pop();
    PSet symmetric_difference(const PSet& other) const;
    void symmetric_difference_update(const PSet& other);
    PSet union_(const PSet& other) const;
    void update(const PSet& other);
    
    PSet operator-(const PSet& other) const;
    PSet operator-(const PAnyWrapper& other) const;
    bool operator==(const PSet& other) const;
    inline bool operator!=(const PSet& other) const {
        return !(*this == other);
    }
    inline bool operator<(const PSet& other) const {
        return issubset(other) && size() < other.size();
    }
    inline bool operator<=(const PSet& other) const {
        return issubset(other);
    }
    inline bool operator>(const PSet& other) const {
        return !(*this <= other);
    }
    inline bool operator>=(const PSet& other) const {
        return !(*this < other);
    }


    using iterator = std::unordered_set<PAnyWrapper, HashFunc>::iterator;
    using const_iterator = std::unordered_set<PAnyWrapper, HashFunc>::const_iterator;
    using value_type = PAnyWrapper;
    using const_value_type = const PAnyWrapper;
    inline PIterator<PSet> begin(){
        return container_.begin();
    }
    inline PIterator<PSet> end(){
        return container_.end();
    }
    inline const PIterator<PSet> cbegin() const{
        return container_.begin();
    }
    inline const PIterator<PSet> cend() const{
        return container_.end();
    }

private:
    virtual void _print(std::ostream& os) const override;
    std::unordered_set<PAnyWrapper, HashFunc> container_;

};
}