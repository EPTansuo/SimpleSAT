#pragma once

#include "clause.hpp"
#include <list>

namespace ssat
{

class Formula{
public:
    Formula(){}
    Formula(Clause clause){
        clauses.push_back(clause);
    }
    void push(Clause clause){
        clauses.push_back(clause);
    }
    std::list<Clause>& getClauses(){
        return clauses;
    }

    bool empty(){
        return clauses.empty();
    }

    std::string toString() const {
        std::string str = "";
        for(int i = 0; i < clauses.size(); i++){
            str += "(";
            auto it = std::next(clauses.begin(), i);
            str += it->toString();
            str += ")";
            if(i != clauses.size() - 1){
                str += " ∧ ";
            }
        }
        return str;
    }
    friend std::ostream& operator<<(std::ostream& os, const Formula& formula){
        os << formula.toString();
        return os;
    }

    bool getResult(){
        for(auto clause : clauses){
            if(!clause.getResult()){
                return false;
            }
            return true;
        }
    }

private: 

    std::list<ssat::Clause> clauses; 
};

} // namespace ssat
