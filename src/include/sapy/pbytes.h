#pragma once

#include "sapy/pobject.h"
#include "sapy/pstring.h"
#include <vector>
#include <cstdint>
#include <iostream>

namespace sapy {

class PBytes : public PObject {
public:
    PBytes() = default;
    PBytes(const std::vector<uint8_t>& data);
    PBytes(const PBytes& other);

    virtual PString toString() const override;
    size_t length() const; 
    size_t size() const;

private:
    virtual void _print(std::ostream& os) const override;
    std::vector<uint8_t> _data;
};

}  // namespace sapy
