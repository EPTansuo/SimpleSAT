#include "sapy/panywrapper.h"
#include "sapy/pstring.h"
#include "sapy/pset.h"
#include <string>
namespace sapy {

PString PAnyWrapper::toString() const {
    const std::type_info& ti = data_.type();

    if (ti == typeid(int)) {
        int val = std::any_cast<int>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(unsigned int)) {
        unsigned int val = std::any_cast<unsigned int>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(long)) {
        long val = std::any_cast<long>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(unsigned long)) {
        unsigned long val = std::any_cast<unsigned long>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(long long)) {
        long long val = std::any_cast<long long>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(unsigned long long)) {
        unsigned long long val = std::any_cast<unsigned long long>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(float)) {
        float val = std::any_cast<float>(data_);
        return PString(std::to_string(val));
    }else if (ti == typeid(double)) {
        double val = std::any_cast<double>(data_);
        return PString(std::to_string(val));
    } else if (ti == typeid(std::string)) {
        return PString(std::any_cast<std::string>(data_));
    } else if (ti == typeid(const char*)) {
        return PString((std::any_cast<const char*>(data_)));
    } else if (ti == typeid(char)) {
        return PString(std::string(1, std::any_cast<char>(data_)));
    } else if (ti == typeid(PString)) {
        return std::any_cast<PString>(data_);
    } else if (ti == typeid(PList)) {
        return std::any_cast<PList>(data_).toString();
    } else if (ti == typeid(PDict)) {
        return std::any_cast<PDict>(data_).toString();
    } else if (ti == typeid(PSet)) {
        return std::any_cast<PSet>(data_).toString();
    }
    return PString(std::string("<type: ") + ti.name() + ">");
}

bool PAnyWrapper::isString() const {
    const std::type_info& ti = data_.type();
    return ti == typeid(std::string) || ti == typeid(const char*) || 
           ti == typeid(char) || ti == typeid(PString) ||
           ti == typeid(char32_t) || ti == typeid(std::u32string);
}

void PAnyWrapper::_print(std::ostream& os) const {
    os << toString();
}

size_t PAnyWrapper::hash() const {
    const std::type_info& ti = data_.type();
    if (ti == typeid(int)) {
        return std::hash<int>{}(std::any_cast<int>(data_));
    } else if (ti == typeid(unsigned int)) {
        return std::hash<unsigned int>{}(std::any_cast<unsigned int>(data_));
    } else if (ti == typeid(long)) {
        return std::hash<long>{}(std::any_cast<long>(data_));
    } else if (ti == typeid(unsigned long)) {
        return std::hash<unsigned long>{}(std::any_cast<unsigned long>(data_));
    } else if (ti == typeid(long long)) {
        return std::hash<long long>{}(std::any_cast<long long>(data_));
    } else if (ti == typeid(unsigned long long)) {
        return std::hash<unsigned long long>{}(std::any_cast<unsigned long long>(data_));
    } else if (ti == typeid(float)) {
        return std::hash<float>{}(std::any_cast<float>(data_));
    }else if (ti == typeid(double)) {
        return std::hash<double>{}(std::any_cast<double>(data_));
    } else if (ti == typeid(std::string)) {
        //return std::hash<std::string>{}(std::any_cast<std::string>(data_));
        return PString(std::any_cast<std::string>(data_)).hash();
    } else if (ti == typeid(const char*)) {
        //return std::hash<std::string>{}(std::any_cast<const char*>(data_));
        return PString(std::any_cast<const char*>(data_)).hash();
    } else if (ti == typeid(char)) {
        return std::hash<char>{}(std::any_cast<char>(data_));
    } else if (ti == typeid(PString)) {
        return std::any_cast<PString>(data_).hash();
    } else if (ti == typeid(PList)) {
        return std::any_cast<PList>(data_).hash();
    } else if (ti == typeid(PDict)) {
        return std::any_cast<PDict>(data_).hash();
    } else if (ti == typeid(PSet)) {
        return std::any_cast<PSet>(data_).hash();
    } else if(ti == typeid(char32_t)) {
        return std::hash<char32_t>{}(std::any_cast<char32_t>(data_));
    } else if(ti == typeid(std::u32string)) {
        return std::hash<std::u32string>{}(std::any_cast<std::u32string>(data_));
    } else {
        return std::hash<std::string>()(ti.name());
    }
}

bool PAnyWrapper::operator==(const PAnyWrapper& other) const {
    const std::type_info& ti1 = data_.type();
    const std::type_info& ti2 = other.data_.type();
    if (ti1 != ti2) {
        return false;
    }
    if(ti1 == typeid(int)){
        return std::any_cast<int>(data_) == std::any_cast<int>(other.data_);
    }else if(ti1 == typeid(unsigned int)){
        return std::any_cast<unsigned int>(data_) == std::any_cast<unsigned int>(other.data_);
    }else if(ti1 == typeid(long)){
        return std::any_cast<long>(data_) == std::any_cast<long>(other.data_);
    }else if(ti1 == typeid(unsigned long)){
        return std::any_cast<unsigned long>(data_) == std::any_cast<unsigned long>(other.data_);
    }else if(ti1 == typeid(long long)){
        return std::any_cast<long long>(data_) == std::any_cast<long long>(other.data_);
    }else if(ti1 == typeid(unsigned long long)){
        return std::any_cast<unsigned long long>(data_) == std::any_cast<unsigned long long>(other.data_);
    }else if(ti1 == typeid(float)){
        return std::any_cast<float>(data_) == std::any_cast<float>(other.data_);
    }else if(ti1 == typeid(double)){
        return std::any_cast<double>(data_) == std::any_cast<double>(other.data_);
    }else if(ti1 == typeid(std::string)){
        return std::any_cast<std::string>(data_) == std::any_cast<std::string>(other.data_);
    }else if(ti1 == typeid(const char*)){
        return std::any_cast<const char*>(data_) == std::any_cast<const char*>(other.data_);
    }else if(ti1 == typeid(char)){
        return std::any_cast<char>(data_) == std::any_cast<char>(other.data_);
    }else if(ti1 == typeid(PString)){
        return std::any_cast<PString>(data_) == std::any_cast<PString>(other.data_);
    }else if(ti1 == typeid(PList)){
        return std::any_cast<PList>(data_) == std::any_cast<PList>(other.data_);
    }else if(ti1 == typeid(PDict)){
        return std::any_cast<PDict>(data_) == std::any_cast<PDict>(other.data_);
    }else if(ti1 == typeid(PSet)){
        return std::any_cast<PSet>(data_) == std::any_cast<PSet>(other.data_);
    }else if(ti1 == typeid(char32_t)){
        return std::any_cast<char32_t>(data_) == std::any_cast<char32_t>(other.data_);
    }else if(ti1 == typeid(std::u32string)){
        return std::any_cast<std::u32string>(data_) == std::any_cast<std::u32string>(other.data_);
    }else {

        // Maybe we should **register a new type** for PAnyWrapper, and use that type to compare
        throw std::runtime_error("Unsupported type for PAnyWrapper == operator:" + std::string(ti1.name()));
        return false;
    }
}

bool PAnyWrapper::operator<(const PAnyWrapper& other) const {
    const std::type_info& ti1 = data_.type();
    const std::type_info& ti2 = other.data_.type();
    if (ti1 != ti2) {
        return ti1.before(ti2);
    }
    if(ti1 == typeid(int)){
        return std::any_cast<int>(data_) < std::any_cast<int>(other.data_);
    }else if(ti1 == typeid(unsigned int)){
        return std::any_cast<unsigned int>(data_) < std::any_cast<unsigned int>(other.data_);
    }else if(ti1 == typeid(long)){
        return std::any_cast<long>(data_) < std::any_cast<long>(other.data_);
    }else if(ti1 == typeid(unsigned long)){
        return std::any_cast<unsigned long>(data_) < std::any_cast<unsigned long>(other.data_);
    }else if(ti1 == typeid(long long)){
        return std::any_cast<long long>(data_) < std::any_cast<long long>(other.data_);
    }else if(ti1 == typeid(unsigned long long)){
        return std::any_cast<unsigned long long>(data_) < std::any_cast<unsigned long long>(other.data_);
    }else if(ti1 == typeid(float)){
        return std::any_cast<float>(data_) < std::any_cast<float>(other.data_);
    }else if(ti1 == typeid(double)){
        return std::any_cast<double>(data_) < std::any_cast<double>(other.data_);
    }else if(ti1 == typeid(std::string)){
        return std::any_cast<std::string>(data_) < std::any_cast<std::string>(other.data_);
    }else if(ti1 == typeid(const char*)){
        return std::any_cast<const char*>(data_) < std::any_cast<const char*>(other.data_);
    }else if(ti1 == typeid(char)){
        return std::any_cast<char>(data_) < std::any_cast<char>(other.data_);
    }else if(ti1 == typeid(PString)){
        return std::any_cast<PString>(data_) < std::any_cast<PString>(other.data_);
    }else if(ti1 == typeid(PList)){
        return std::any_cast<PList>(data_) < std::any_cast<PList>(other.data_);
    }else if(ti1 == typeid(PDict)){
        return std::any_cast<PDict>(data_) < std::any_cast<PDict>(other.data_);
    }else if(ti1 == typeid(PSet)){
        return std::any_cast<PSet>(data_) < std::any_cast<PSet>(other.data_);
    }else if(ti1 == typeid(char32_t)){
        return std::any_cast<char32_t>(data_) < std::any_cast<char32_t>(other.data_);
    }else if(ti1 == typeid(std::u32string)){
        return std::any_cast<std::u32string>(data_) < std::any_cast<std::u32string>(other.data_);
    }else {
        throw std::runtime_error("Unsupported type for PAnyWrapper < operator:" + std::string(ti1.name()));
        return false;
    }
}

bool PAnyWrapper::operator>(const PAnyWrapper& other) const {
    return other < *this && *this != other;
}
    

} // namespace sapy
