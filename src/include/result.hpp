#pragma once

#include <string>

namespace ssat{

class Result {
public:
    static const Result SAT;
    static const Result UNSAT;
    static const Result UNKNOWN;
    static const Result ERROR;

    std::string toString() const {
        switch (type) {
        case Type::SAT: return "SAT";
        case Type::UNSAT: return "UNSAT";
        case Type::UNKNOWN: return "UNKNOWN";
        case Type::ERROR: return "ERROR";
        default: return "INVALID";
        }
    }

    bool operator==(const Result& other) const {
        return type == other.type;
    }

    bool operator!=(const Result& other) const {
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

const Result Result::SAT(Result::Type::SAT);
const Result Result::UNSAT(Result::Type::UNSAT);
const Result Result::UNKNOWN(Result::Type::UNKNOWN);
const Result Result::ERROR(Result::Type::ERROR);

}