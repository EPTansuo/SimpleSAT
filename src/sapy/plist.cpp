#include "sapy/plist.h"
namespace sapy{


void PList::_print(std::ostream& os) const{
    os << toString();
}

PString PList::toString() const {
    PString result = "[";
    for (size_t i = 0; i < data_.size(); i++) {
        if(data_[i].isString()){
            result += "\"";
            result += data_[i].toString();
            result += "\"";
        }else{
        result += data_[i].toString();
        }
        if (i + 1 < data_.size()) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

bool PList::operator==(const PList& other) const {
    if (data_.size() != other.data_.size()) {
        return false;
    }
    for (size_t i = 0; i < data_.size(); i++) {
        if (data_[i] != other.data_[i]) {
            return false;
        }
    }
    return true;
}

void PList::sort(
    std::function<bool(const PAnyWrapper&, const PAnyWrapper&)> cmp,
    std::function<PAnyWrapper(const PAnyWrapper&)> key,
    bool reverse
){
    sortT<PAnyWrapper>(cmp, key, reverse);
}


bool PList::operator<(const PList& other) const {
    if(data_.size() < other.data_.size()){
        return true;
    }
    for(size_t i = 0; i < data_.size(); i++){
        if(data_[i] < other.data_[i]){
            return true;
        }
    }
    return false;
}

}

