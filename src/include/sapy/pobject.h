#pragma once

#include <ostream>

namespace sapy{

class PString;

class PObject{
public:
    PObject(){}
    virtual ~PObject() {}
    virtual PString toString() const = 0;
    virtual size_t hash() const{
            return reinterpret_cast<size_t>(this);
    }

    // virtual bool operator==(const PObject &other) const{
    //     return hash() == other.hash();
    // }

    // bool operator!=(const PObject &other) const{
    //     return !(*this == other);
    // }

private:  
    virtual void _print(std::ostream& os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const PObject& obj);

};


}