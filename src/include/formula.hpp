#pragma once

#include <sapy/pset.h>
#include <iostream>

namespace ssat
{

class Formula{
public:
    Formula(){}
    Formula(const sapy::PSet& formula): formula_(formula) {}

    sapy::PSet& getFormula() {
        return formula_;
    }

    sapy::PString toPString() const {
        sapy::PString result = "Formula: ";
        size_t i=0, j=0; 
        for(auto clause = formula_.cbegin(); clause != formula_.cend(); clause++){
            i++;
            result += "(";
            j=0;
            for(sapy::PString literal : clause->unwrap<sapy::PSet>()){
                j++;
                result += (literal + (j != clause->unwrap<sapy::PSet>().size() ? " ∨ " : ""));
            }
            result += (sapy::PString(")") + (i != formula_.size() ? " ∧ " : ""));
        }
        return result;
    }
    inline void clear(){
        formula_.clear();
    }
    inline void add(const sapy::PSet& clause){
        formula_.add(clause);
    }

private: 
    friend std::ostream& operator<<(std::ostream& os, const Formula& formula){
        os << formula.toPString();
        return os;
    }
    sapy::PSet formula_;

};

} // namespace ssat
