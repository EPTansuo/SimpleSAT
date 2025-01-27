#pragma once

#include <string>

namespace ssat{

class Result {
public:
    static const Result SAT;
    static const Result UNSAT;
    static const Result UNKNOWN;
    static const Result ERROR;

    inline std::string toString() const {
        switch (type) {
        case Type::SAT: return "SAT";
        case Type::UNSAT: return "UNSAT";
        case Type::UNKNOWN: return "UNKNOWN";
        case Type::ERROR: return "ERROR";
        default: return "INVALID";
        }
    }

    inline bool operator==(const Result& other) const {
        return type == other.type;
    }

    inline bool operator!=(const Result& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Result& result){
        os << result.toString();
        return os;
    }

private:
    enum class Type { SAT, UNSAT, UNKNOWN, ERROR };
    Type type;
    Result(Type t) :type(t) {}
};

inline const Result Result::SAT(Result::Type::SAT);
inline const Result Result::UNSAT(Result::Type::UNSAT);
inline const Result Result::UNKNOWN(Result::Type::UNKNOWN);
inline const Result Result::ERROR(Result::Type::ERROR);

}