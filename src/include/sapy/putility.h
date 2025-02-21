#pragma once
#include "sapy/pobject.h"
#include "sapy/pcollection_interface.h"
#include "sapy/pstring.h"

namespace sapy{

// Determine whether it is the same object (rather than value)
bool is(const PObject &a, const PObject &b);

// hash function
size_t hash(const PObject& obj);

template<typename T>
size_t len(const PCollectionInterface<T>& collection){
    return collection.size();
}

size_t len(const PString& str);

}