#pragma once
// #include <sapy/panywrapper.h>
#include <cstddef>
#include <utility>
namespace sapy{

// class PEnumeratorInterface{
// private:
//     PAnyWrapper current_;
// public:
//     PAnyWrapper& getCurrent(){
//         return current_;
//     }
// };

// class PEnumerableInterface{
//     virtual PEnumerableInterface& getEnumerator() = 0;
// };



/**
 * The generic parameter here (Derived) refers
 * to the object that implements the interface.
 */
template <typename Derived>
class PCollectionInterface{
public:
    // add item to container back
    template<typename... Args>
    void append(Args&&... args){
        static_cast<Derived *>(this)->appendImpl(std::forward<Args>(args)...);
    }

    // clear container
    void clear(){
        static_cast<Derived *>(this)->clearImpl();
    }

    template<typename T>
    bool remove(const T& elem){
        return static_cast<Derived *>(this)->removeImpl(elem);
    }

    template<typename T>
    bool contains(const T& elem) const {
        return static_cast<const Derived *>(this)->containImpl(elem);
    }

    size_t size() const{
        return static_cast<const Derived *>(this)->sizeImpl();
    }
};
}