#pragma once

#include <string>
#include <memory>

namespace ssat{


class Literal {

public: Literal(std::string name, bool negated, bool assigned=false) 
        : name(name),negated(negated),assigned(assigned){}

    Literal(){};
    std::string getName() {
        return name;
    }
    void setName(std::string name) {
        this->name = name;
    }
    bool isNegated() {
        return negated;
    }
    void setNegated(bool negated) {
        this->negated = negated;
    }
    std::string toString() const {
        return negated ? "¬" + name : name;
    }

    friend std::ostream& operator<<(std::ostream& os, const Literal& var){
        os << var.toString();
        return os;
    }

    void setValue(bool value) {
        this->value = value;
        assigned = true;
    }

    void unsetValue() {
        assigned = false;
    }

    bool getResult() {
        if(!assigned){
            throw std::runtime_error("Variable not assigned");
        }
        if(negated){
            return !value;
        }
        return value;
    }
    bool isAssigned() {
        return assigned;
    }
    bool operator==(const Literal& other) const {
        return name == other.name && negated == other.negated && 
        value == other.value && assigned == other.assigned;
    }
private:
    std::string name;
    bool negated;
    bool value;
    bool assigned;
};


}