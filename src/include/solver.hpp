#pragma once 

#include "result.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <log.hpp>
#include <sapy/pstring.h>
#include <sapy/pset.h>
#include <sapy/plist.h>
#include <variant>
#include <array>
namespace ssat{

using Literal = sapy::PString;
using Clause = sapy::PSet;
using Clauses = sapy::PSet;
using Formula = sapy::PSet;
using Variable = sapy::PString;

/******************************************************
 * The Data Structure of BDD based on the concepts from Page 15
 * "Introduction to Binary Decision Diagrams" by Henrik Reif Andersen.
 *
 * - BDDNodeVar: [index, low, high]
 * - BDDNode: std::pair<Variable, BDDNodeVar>
 * - BDD: var_name ⟼ [index, low, high] (mapping)
 ******************************************************/
using BDDNode = std::pair<sapy::PString, sapy::PList>;
using BDDNodeVar = sapy::PList; 
using BDD = sapy::PDict;


#define METHOD_NAMES(_) \
    _(DP) \
    _(DPLL) \
    _(DPLL_CLASSIC) \
    _(CDCL) \
    _(SYMBOLIC_SAT) \
    _(RESOLUTION)

#define ENUM_GEN(name) name,
#define STRING_GEN(name) sapy::PString(#name),


class Solver {
public: 
    Solver() {}
    Solver(const sapy::PString _filename): filename_(_filename) {}

    enum Method {
        METHOD_NAMES(ENUM_GEN)
        METHOD_COUNT
    };

    static const sapy::PList MethodNames;


    static sapy::PString MethodtoString(Method method) {
        if (method < METHOD_COUNT) {
            return MethodNames[method];
        }
        return "Unknown";
    }

    static Method methodFromString(const sapy::PString& methodName) {
        for (size_t i = 0; i < MethodNames.size(); ++i) {
            if (methodName == MethodNames[i]) {
                return static_cast<Method>(i);
            }
        }
        throw std::invalid_argument("Invalid method name: " + methodName);
    }

    void readCNF(const sapy::PString filename);
    inline void readCNF(){readCNF(filename_);}
    Result solve(Method method = DPLL);
    
    Formula& getFormula() {
        return formula_;
    }
    size_t getValCnt() const {
        return val_cnt_;
    }
    sapy::PString toString() const {
        return "Solver: <" + formulaToPString() + ">";
    }
    void setVariableOrder(std::function<bool(const Variable&, const Variable&)> order){
        varibale_order_ = order;
    }

    sapy::PString formulaToPString() const {
        sapy::PString result;
        size_t i=0, j=0; 
        for(auto clause = formula_.cbegin(); clause != formula_.cend(); clause++){
            i++;
            result += "(";
            j=0;
            for(sapy::PString literal : clause->unwrap<Clause>()){
                j++;
                result += (literal + (j != clause->unwrap<Clause>().size() ? " ∨ " : ""));
            }
            result += (sapy::PString(")") + (i != formula_.size() ? " ∧ " : ""));
        }
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Solver& solver){
        os << solver.toString();
        return os;
    }
private:    
    sapy::PString filename_;
    Formula formula_;
    sapy::PList sorted_variables_;
    bool digit_variable_;
    size_t val_cnt_;

    // bool UP(Formula &);
    // bool PLE(Formula &);
    // Result _solve_DPLL(Formula);

    Result _solve_Resolution(Formula,sapy::PSet literals);
    Clause _resolve(const Clause clause1, const Clause clause2, Literal& literal) const;
    Clause _resolve(const Clause clause1, const Clause clause2, const Variable& variable, bool& got) const;
    // void _condition(Formula& formula, const Literal& literal) const;
    Formula _condition(const Formula formula, const Literal& literal) const;
    Literal _complementry(const Literal& literal) const;

    Result _solve_DP(Formula formula);
    Variable _firstVariable(const Clause& clause) const;

    std::variant<Result, Clause> _solve_DPLL_r(Formula formula, size_t depth) const ;
    Result _solve_DPLL(Formula formula) const;
    Result _solve_DPLL_classic(Formula formula) const;
    Formula _unit_progate(Formula formula) const;
    Formula _pure_iteral_elimination(Formula formula) const;

    Result _solve_SYMBOLIC_SAT(Formula formula) const;

    BDD _OBDD_r(BDD bdd, Clause clause, int branch = 0) const;
    BDD _OBDD(Clause clause) const;
    // BDD _ROBDD(Clause clause) const;

    sapy::PString _BDD_to_dot(const BDD& bdd) const;
    void _show_dot(const sapy::PString& dot, bool block = true) const;
    void _show_BDD(const BDD& bdd, bool block = true) const;
    BDD _reduce_OBDD(const BDD& bdd)const;


    // indice here is 1-based
    int _variable2indice(const Variable& variable) const;
    Variable _indice2variable(int indice) const;


    std::function<bool(const Variable&, const Variable&)> varibale_order_ = [](const Variable& v1, const Variable& v2){
        return v1 < v2;
    };
};


inline const sapy::PList Solver::MethodNames = {
        METHOD_NAMES(STRING_GEN)
};

inline Result Solver::_solve_Resolution(Formula formula, sapy::PSet literals){
    LOG_DEBUG("Formula: {}", formula.toString());

    sapy::PList to_add;
    sapy::PList to_remove;  

    bool to_stop = false;

    for(auto it = formula.begin(); it != formula.end() && (!to_stop); it++){
        for(auto jt = formula.begin(); jt != formula.end() && (!to_stop); jt++){
            if(it == jt) continue;
            Clause clause1 = *it;
            Clause clause2 = *jt;
            //LOG_DETAIL("for : Clause1: {}, Clause2: {}", clause1.toString(),
            //                                      clause2.toString());
            Literal l;                                                  
            Clause resolvent = _resolve(clause1, clause2, l);
            
            if(l.length() == 0){
                continue;
            }

            literals.add(l);
            
            to_add.append(resolvent);
            to_remove.append(clause1, clause2);
            LOG_DEBUG("Clause1: {}, Clause2: {}", clause1.toString(),
                                                    clause2.toString());
            LOG_DEBUG("Resolvent: {}", resolvent.toString());
            // to_stop = true;

        }
    }
    
    for(auto it = to_add.begin(); it != to_add.end(); it++){
        //LOG_DEBUG("Add: {}", it->toString());
        formula.add(*it);
    }

    for(auto it = to_remove.begin(); it != to_remove.end(); it++){
        //LOG_DEBUG("Remove: {}", it->toString());
        formula.discard(*it);
    }
    
    LOG_DETAIL("Literals of resolution: {}", literals.toString());

    if(to_remove.size()==0){
        // Stop  recursive here
        if(formula.contain(sapy::PSet())){
            return Result::UNSAT;   
        }else if(formula.size() == 1){
            return Result::SAT;
        }else{
            return Result::UNKNOWN;
            // for(auto literal_wrap: literals){
            //     _condition(formula, literal_wrap);
            //     if(formula.contain(sapy::PSet())){
            //         return Result::UNSAT;
            //     }else{
            //         return Result::UNKNOWN;
            //     }
            // }
        }
    }
    else{
        return _solve_Resolution(formula,literals);
    }

    
    
    return Result::UNKNOWN;
}

}

