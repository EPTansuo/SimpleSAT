#include "sapy/putility.h"

namespace sapy{
bool is(const PObject &a, const PObject &b){
    return a.hash() == b.hash();
}

size_t hash(const PObject &obj){
    return obj.hash();
}

size_t len(const PString &str){
    return str.length();
}


}