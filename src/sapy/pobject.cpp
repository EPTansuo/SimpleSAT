#include "sapy/pobject.h"
#include <ostream>

namespace sapy{
std::ostream& operator<<(std::ostream& os, const PObject& obj) {
    obj._print(os);
    return os;
}

}