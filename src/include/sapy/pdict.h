#pragma once 

#include "sapy/pobject.h"
#include "sapy/pcollection_interface.h"
#include "sapy/pstring.h"
#include <unordered_map>
#include "sapy/putility.h"
#include <initializer_list>
#include "sapy/piterator.h"
#include "sapy/panywrapper.h"

namespace sapy{

class PAnyWrapper;

class PDict : public PObject, public PCollectionInterface<PDict>{
private:
    friend class PCollectionInterface<PDict>;

/*****************Implement PCollectionInterface methods*******************/
private:
    template <typename... Args>
    inline void appendImpl(Args &&...args){
        (container_.emplace(std::forward<Args>(args)), ...);
    }

    inline void clearImpl(){
        container_.clear();
    }

    template <typename T>
    inline bool removeImpl(const T &elem){
        auto it = std::find(container_.begin(), container_.end(), elem);
        if (it == container_.end())
            return false;

        container_.erase(it);
        return true;
    }

    inline size_t sizeImpl(){
        return container_.size();
    }

    template <typename T>
    inline bool containImpl(const T &elem) const{
        auto res = std::find(container_.begin(), container_.end(), elem);
        return res != container_.end();
    }
    struct HashFunc {
        size_t operator()(const PAnyWrapper &obj) const;
    };
public:
    PDict() = default;
    PDict(std::initializer_list<std::pair<PAnyWrapper, PAnyWrapper>> init);
    virtual PString toString() const override;
    PAnyWrapper& operator[](const PAnyWrapper& key);
    const PAnyWrapper& operator[](const PAnyWrapper& key) const;

    bool operator==(const PDict& other) const;
    inline bool operator!=(const PDict& other) const {
        return !(*this == other);
    }

    inline bool operator<(const PDict& other) const {
        throw std::runtime_error("Unsupported type for PDict < operator");
        return false;
    }
    inline bool operator>(const PDict& other) const {
        throw std::runtime_error("Unsupported type for PDict > operator");
        return false;
    }
    inline bool operator<=(const PDict& other) const {
        throw std::runtime_error("Unsupported type for PDict <= operator");
        return false;
    }
    inline bool operator>=(const PDict& other) const {
        throw std::runtime_error("Unsupported type for PDict >= operator");
        return false;
    }
    
    using iterator = std::unordered_map<PAnyWrapper, PAnyWrapper, HashFunc>::iterator;
    using const_iterator = std::unordered_map<PAnyWrapper, PAnyWrapper, HashFunc>::const_iterator;  
    using value_type = std::pair<PAnyWrapper, PAnyWrapper>;
    using const_value_type = const std::pair<const PAnyWrapper, PAnyWrapper>;
    inline PIterator<PDict> begin(){
        return (container_.begin());
    }

    inline PIterator<PDict> end(){
        return (container_.end());
    }
    inline const PIterator<PDict> begin() const{
        return (container_.cbegin());
    }
    inline const PIterator<PDict> end() const{
        return (container_.cend());
    }

    inline iterator find(const PAnyWrapper& key){
        return iterator(container_.find(key));
    }
    

    inline const_iterator find(const PAnyWrapper& key) const{
        return const_iterator(container_.find(key));
    }

    bool contains(const PAnyWrapper& key) const;

private:
    
    virtual void _print(std::ostream &os) const override;
    std::unordered_map<PAnyWrapper, PAnyWrapper, HashFunc> container_;
};

}