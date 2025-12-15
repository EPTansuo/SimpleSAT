#pragma once 

#include <string>
#include <vector>
#include <stdexcept>
#include <tuple>

namespace ssat {

#define SSAT_METHOD_TABLE(X) \
    X(DP,              DP_Solver) \
    X(DPLL,            DPLL_Solver) \
    X(CDCL,            CDCL_Solver) \

enum class Method {
#define ENUM_ONLY(method, solver) method,
    SSAT_METHOD_TABLE(ENUM_ONLY)
    COUNT
#undef ENUM_ONLY
};


#define NAME_ONLY(method, solver) std::string(#method),
inline const std::vector<std::string> MethodNames = { SSAT_METHOD_TABLE(NAME_ONLY) };
inline constexpr auto MethodChoicesTuple = std::tuple{ SSAT_METHOD_TABLE(NAME_ONLY)};
#undef NAME_ONLY


inline std::string method_to_string(Method method) {
    size_t index = static_cast<size_t>(method);
    if (index >= MethodNames.size()) {
        throw std::runtime_error("method_to_string: invalid method enum value");
    }
    return MethodNames[index];
}

inline Method method_from_string(const std::string& name) {
    for (size_t i = 0; i < MethodNames.size(); ++i) {
        if (MethodNames[i] == name) {
            return static_cast<Method>(i);
        }
    }
    throw std::runtime_error("method_from_string: unknown method name: " + name);
}


} // namespace ssat
