#pragma once


#include <string>
#include <list>
#include "literal.hpp"

namespace ssat{


class Clause {

public: 
    Clause(){}
    Clause(Literal literal){
        literals.push_back(literal);
    }
    void push(Literal literal) {
        literals.push_back(literal);
    }


    std::list<Literal>& getLiterals(){
        return literals;
    }

    std::string toString() const {
        std::string str = "";
        for(int i = 0; i < literals.size(); i++){
            auto it = std::next(literals.begin(), i);
            str += it->toString();
            if(i != literals.size() - 1){
                str += " ∨ ";
            }
        }
        return str;
    }
    friend std::ostream& operator<<(std::ostream& os, const Clause& clause){
        os << clause.toString();
        return os;
    }

    bool getResult(){
        for(auto literal : literals){
            if(literal.getResult()){
                return true;
            }
        }
        return false;
    }
    bool operator==(const Clause& other) const {
        return literals == other.literals;
    }
private:
std::list<Literal> literals;
};



}