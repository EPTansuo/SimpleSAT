#pragma once

#include <memory>
#include <type_traits> // std::is_arithmetic_v
#include <utility>     // std::move
#include <any>
#include <typeinfo>
#include <stdexcept>
#include "sapy/pobject.h"
#include "sapy/pstring.h"

namespace sapy{

class PAnyWrapper : public PObject{
public:
    PAnyWrapper() = default;
    template <typename T>
    PAnyWrapper(T val)
        : data_(std::move(val)) {}



    template <typename T>
    T unwrap() const{
        if (!data_.has_value()) {
            std::cerr << "bad any cast: std::any is empty" << std::endl;
            throw std::bad_cast();
        }
        try {
            return std::any_cast<T>(data_);
        } catch (const std::bad_any_cast&) {
            const std::type_info& ti = data_.type();
            std::cerr << "bad any cast: " << ti.name() << std::endl;
            throw std::bad_cast();
        }
    }
    virtual size_t hash() const override;

    template <typename U>
    operator U() const{
        const std::type_info& ti = data_.type();

        if (ti == typeid(const char*)) {
            if constexpr (std::is_same<U, PString>::value) {
                return PString(unwrap<const char*>());
            }
            if constexpr (std::is_convertible<const char*, U>::value) {
                return unwrap<const char*>(); 
            }
            throw std::bad_cast(); 
        }

        if (ti == typeid(std::string)) {
            if constexpr (std::is_same<U, PString>::value) {
                return PString(unwrap<std::string>()); 
            }
            if constexpr (std::is_convertible<std::string, U>::value) {
                return unwrap<std::string>(); 
            }
            throw std::bad_cast(); 
        }

        return unwrap<U>();
    }

    template <typename T>
    bool is() const {
        return data_.type() == typeid(T);
    }

    bool operator==(const PAnyWrapper& other) const;
    inline bool operator!=(const PAnyWrapper& other) const {
        return !(*this == other);
    }

    bool operator<(const PAnyWrapper& other) const;
    bool operator>(const PAnyWrapper& other) const;
    inline bool operator<=(const PAnyWrapper& other) const{
        return !(*this > other);
    }
    inline bool operator>=(const PAnyWrapper& other) const{
        return !(*this < other);
    }
    virtual PString toString() const override;
    bool isString() const;
    PString getString() const;
private: 
    virtual void _print(std::ostream& os) const override;
    std::any data_;
};

}