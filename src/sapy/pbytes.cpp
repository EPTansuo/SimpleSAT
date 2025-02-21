#include "sapy/pbytes.h"

namespace sapy {

PBytes::PBytes(const std::vector<uint8_t>& data)
    : _data(data) {}

PBytes::PBytes(const PBytes& other)
    : _data(other._data) {}

PString PBytes::toString() const {
    PString result = "b\"";
    for (auto c : _data) {
        if (c >= 32 && c <= 126) {
            result += static_cast<char>(c);
        } else {
            char buffer[5];
            snprintf(buffer, sizeof(buffer), "\\x%02x", c);
            result += buffer;
        }
        
    }
    result += "\"";
    return result;
}

size_t PBytes::length() const {
    return _data.size();
}
size_t PBytes::size() const {
    return _data.size();
}

void PBytes::_print(std::ostream& os) const {
    os << toString();
}

}  // namespace sapy
