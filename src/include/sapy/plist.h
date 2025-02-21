#pragma once
#include "sapy/panywrapper.h"
#include "sapy/pcollection_interface.h"
#include "sapy/piterator.h"
// #include "sapy/pstring.h"
#include <algorithm>
#include <functional>

namespace sapy{
class PString;
class PAnyWrapper;
class PList : public PObject, public PCollectionInterface<PList> {
private:
    template<typename... Args>
    void appendImpl(Args&&... args) {
        (data_.emplace_back(std::forward<Args>(args)), ...);
    }

    inline void clearImpl(){
        data_.clear();
    }

    template <typename T>
    inline bool removeImpl(const T &elem){
        auto it = std::find(data_.begin(), data_.end(), elem);
        if(it == data_.end()) return false;

        data_.erase(it);
        return true;
    }

    inline size_t sizeImpl() const{
        return data_.size(); 
    }


    template <typename T>
    inline bool containImpl(const T &elem) const{
        auto res = std::find(data_.begin(), data_.end(), elem);
        return res != data_.end();
    }

/***********************override operators************************ */
public:
    inline PAnyWrapper &operator[](size_t index){
        return data_[index];
    }

    inline const PAnyWrapper &operator[](size_t index) const{
        return data_[index];
    }

    bool operator==(const PList &other) const;

    inline bool operator!=(const PList &other) const{
        return !(*this == other);
    }

    PList operator+(const PList& other) const{
        std::vector<PAnyWrapper> result;
        result.insert(result.end(), data_.begin(), data_.end());
        result.insert(result.end(), other.data_.begin(), other.data_.end());
        return PList(std::move(result));
    }

    PList& operator+=(const PList& other){
        this->extend(other);
        return *this;
    }

    PList& operator=(const PList& other){
        return *this;
    }

    bool operator<(const PList& other) const;

    inline bool operator<=(const PList& other) const{
        return (*this)<other || (*this)==other;
    }

    inline bool operator>(const PList& other) const{
        return (!((*this)<other)) && !(*this == other);
    }

    inline bool operator>=(const PList& other) const{
        return !((*this)<other);
    }


public:

    PList() = default;
    PList(const std::vector<PAnyWrapper>& init) : data_(init.begin(), init.end()) {}
    PList(const std::initializer_list<PAnyWrapper>& init) : data_(init){};
    // template <typename... Args>
    // PList(Args&&... args) {
    //     (data_.emplace_back(std::forward<Args>(args)), ...);
    // }

    PString toString() const;

    
    using iterator = std::vector<PAnyWrapper>::iterator;
    using const_iterator = std::vector<PAnyWrapper>::const_iterator;  
    using value_type = PAnyWrapper;
    using const_value_type = const PAnyWrapper;
    inline PIterator<PList> begin(){
        return (data_.begin());
    }

    inline PIterator<PList> end(){
        return (data_.end());
    }
    inline const PIterator<PList> cbegin() const{
        return (data_.cbegin());
    }
    inline const PIterator<PList> cend() const{
        return (data_.cend());
    }

    template <typename T>
    size_t count(const T& elem) const {
        return std::count(data_.begin(), data_.end(), elem);
    }

    void extend(const PList& other){
        data_.insert(data_.end(), other.data_.begin(), other.data_.end());
    }

    void extend(const std::initializer_list<PAnyWrapper>& init){
        data_.insert(data_.end(), init);
    }

    inline void appendSingle(const PAnyWrapper &item){
        data_.emplace_back(item);
    }

    template <typename T>
    size_t index(const T& elem, size_t start, size_t end) const {
        auto it = find(data_.begin() + start, data_.begin() + end, elem);
        if (it == data_.end())
            throw std::runtime_error("The elem is not in list.");

        return it - data_.begin() + start;
    }

    template <typename T>
    size_t index(const T& elem, size_t start) const {
        return index(elem, start, data_.size());
    }


    template <typename T>
    size_t index(const T& elem) const{
        return index(elem, 0, data_.size());
    }

    template <typename T>
    void insert(size_t pos, const T& elem){
        data_.insert(data_.begin() + pos, elem);
    }

    void reverse(){
        std::reverse(data_.begin(), data_.end());
    }

    template <typename T>
    inline void sortT(
        std::function<bool(const T&, const T&)> cmp = nullptr,
        std::function<T(const T&)> key = nullptr,
        bool reverse = false
    ) {
        if (key) {
            if (cmp) {
                std::sort(data_.begin(), data_.end(), [&](const T& a, const T& b) {
                    T key_a = key(a);
                    T key_b = key(b);
                    return reverse ? cmp(key_b, key_a) : cmp(key_a, key_b);
                });
            } else {
                std::sort(data_.begin(), data_.end(), [&](const T& a, const T& b) {
                    T key_a = key(a);
                    T key_b = key(b);
                    return reverse ? key_b < key_a : key_a < key_b;
                });
            }
        } else {
            if (cmp) {
                std::sort(data_.begin(), data_.end(), [&](const T& a, const T& b) {
                    return reverse ? cmp(b, a) : cmp(a, b);
                });
            } else {
                if (reverse) {
                    std::sort(data_.begin(), data_.end(), std::greater<T>());
                } else {
                    std::sort(data_.begin(), data_.end(), std::less<T>());
                }
            }
        }
    }

    void sort(
        std::function<bool(const PAnyWrapper&, const PAnyWrapper&)> cmp = nullptr,
        std::function<PAnyWrapper(const PAnyWrapper&)> key = nullptr,
        bool reverse = false
    );

private:
    virtual void _print(std::ostream &os) const override;

private:
    std::vector<PAnyWrapper> data_; // container
    
    friend class PCollectionInterface<PList>;   // implement Collection interface
};



}