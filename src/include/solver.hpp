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

namespace ssat{

using Literal = sapy::PString;
using Clause = sapy::PSet;
using Clauses = sapy::PSet;
using Formula = sapy::PSet;
using Variable = sapy::PString;

class Solver {
public: 
    Solver() {}
    Solver(const sapy::PString _filename): filename_(_filename) {}

    enum Method {
        DP,
        DPLL,
        CDCL,
        RESOLUTION
    };
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
    size_t val_cnt_;

    // bool UP(Formula &);
    // bool PLE(Formula &);
    // Result _solve_DPLL(Formula);

    Result _solve_Resolution(Formula,sapy::PSet literals);
    Clause _resolve(const Clause clause1, const Clause clause2, Literal& literal) const;
    Clause _resolve(const Clause clause1, const Clause clause2, const Variable& variable, bool& got) const;
    void _condition(Formula& formula, const Literal& literal) const;
    Literal _complementry(const Literal& literal) const;

    Result _solve_DP(Formula formula);
    Variable _firstVariable(const Clause& clause) const;
};

Literal Solver::_complementry(const Literal& literal) const {
    if(literal.startswith("¬")){
        return literal.substr(1);
    }else{
        return "¬" + literal;
    }
}

void Solver::_condition(Formula& formula, const Literal& literal) const {
    LOG_DEBUG("Before Conditioning: {}, Literal: {}", formula.toString().toStdString(),
                                                      literal.toString().toStdString());    
    Literal complementary = _complementry(literal);
    sapy::PList to_add;
    sapy::PList to_remove;
    for (auto it = formula.begin(); it != formula.end(); it++) {
        Clause clause = *it;
        if (clause.contain(literal)) {
            to_remove.append(clause);  
        } else if (clause.contain(complementary)) {
            to_remove.append(clause);
            to_add.append(clause - complementary);
        } else {
            // Do nothing
        }
    }
    for(auto it = to_remove.begin(); it != to_remove.end(); it++){
        formula.discard(*it);
    }

    for(auto it = to_add.begin(); it != to_add.end(); it++){
        formula.add(*it);
    }
    LOG_DEBUG("After Conditioning: {}", formula.toString().toStdString());
}


Clause Solver::_resolve(const Clause clause1, const Clause clause2, const Variable& variable, bool& got) const{
    got = false;
    auto resolvent = Clause();
    LOG_DETAIL("Resolve: Clause1: {}, Clause2: {} with Variable: {}", clause1.toString().toStdString(),
                                                  clause2.toString().toStdString(), variable.toStdString());
    Literal complementary = _complementry(variable);
    if(clause1.contain(variable) && clause2.contain(complementary)){
        resolvent = (clause1-variable).union_(clause2-complementary);
        got = true;
        
    }else if(clause1.contain(complementary) && clause2.contain(variable)){
        resolvent = (clause1-complementary).union_(clause2-variable);
        got = true;
    }

    if(got){
        LOG_DETAIL("{}-Resolvent: {}", variable.toStdString(), resolvent.toString().toStdString());
        return resolvent;
    }    

    return Clause();
}

Clause Solver::_resolve(const Clause clause1, const Clause clause2, Literal& literal) const{
    auto resolvent = Clause();
    for(auto it = clause1.cbegin(); it != clause1.cend(); it++){
        literal = *it;
        Literal complementary = _complementry(literal);
        if(clause2.contain(complementary)){
            resolvent = (clause1-literal).union_(clause2-complementary);
            LOG_DETAIL("Resolvent: {}, Literal: {}", resolvent.toString().toStdString(),
                                                      literal.toString().toStdString());
            return resolvent;
        }
    }
    literal = Literal();
    return Clause();
}



Result Solver::_solve_Resolution(Formula formula, sapy::PSet literals){
    LOG_DEBUG("Formula: {}", formula.toString().toStdString());

    sapy::PList to_add;
    sapy::PList to_remove;  

    bool to_stop = false;

    for(auto it = formula.begin(); it != formula.end() && (!to_stop); it++){
        for(auto jt = formula.begin(); jt != formula.end() && (!to_stop); jt++){
            if(it == jt) continue;
            Clause clause1 = *it;
            Clause clause2 = *jt;
            //LOG_DETAIL("for : Clause1: {}, Clause2: {}", clause1.toString().toStdString(),
            //                                      clause2.toString().toStdString());
            Literal l;                                                  
            Clause resolvent = _resolve(clause1, clause2, l);
            
            if(l.length() == 0){
                continue;
            }

            literals.add(l);
            
            to_add.append(resolvent);
            to_remove.append(clause1, clause2);
            LOG_DEBUG("Clause1: {}, Clause2: {}", clause1.toString().toStdString(),
                                                    clause2.toString().toStdString());
            LOG_DEBUG("Resolvent: {}", resolvent.toString().toStdString());
            // to_stop = true;

        }
    }
    
    for(auto it = to_add.begin(); it != to_add.end(); it++){
        //LOG_DEBUG("Add: {}", it->toString().toStdString());
        formula.add(*it);
    }

    for(auto it = to_remove.begin(); it != to_remove.end(); it++){
        //LOG_DEBUG("Remove: {}", it->toString().toStdString());
        formula.discard(*it);
    }
    
    LOG_DETAIL("Literals of resolution: {}", literals.toString().toStdString());

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


// first variable of C according to order π: sort according to lexicographical order.
Variable Solver::_firstVariable(const Clause& clause) const{
    if(clause.size() == 0){
        std::runtime_error("Empty clause");
        return Variable();
    }
    sapy::PSet unsorted;
    std::vector <Literal> sorted;
    for(auto it = clause.cbegin(); it != clause.cend(); it++){
        Literal literal = *it;
        if(literal.startswith("¬")){
            unsorted.add(literal.substr(1));
        }else{
            unsorted.add(literal);
        }
    }
    for(auto it = unsorted.begin(); it != unsorted.end(); it++){
        sorted.push_back(it->unwrap<Variable>());
    }
    std::sort(sorted.begin(), sorted.end());

    return sorted[0];
}


Result Solver::_solve_DP(Formula formula){
    LOG_DEBUG("Formula: {}", formula.toString().toStdString());
    if(formula.size() == 1 && formula.contain(sapy::PSet())){
        return Result::UNSAT;
    }
    sapy::PDict Buckets;
    
    // for each variable V of Δ do
    for(size_t i=1; i<=val_cnt_; i++){
        Buckets[Variable(std::to_string(i))] = Clauses();  //create empty bucket Bv
    }

    // for each clause C of Δ do
    for(auto clause_wrap: formula){
        Clause clause = clause_wrap;
        Variable variable = _firstVariable(clause); //V = first variable of C according to order π

        // Bv = Bv ∪ {C}
        Clauses C;
        C.add(clause);
        Buckets[variable] = Buckets[variable].unwrap<Clauses>().union_(C);
    }
    LOG_DEBUG("Buckets: {}", Buckets.toString().toStdString());
    // for each variable V of Δ in order π 
    for(size_t i=1; i<=val_cnt_; i++){
        Variable variable = std::to_string(i);
        Clauses bucket = Buckets[variable].unwrap<Clauses>();
        LOG_DETAIL("Variable: {}, Bucket: {}", variable.toStdString(), bucket.toString().toStdString());
        // if BV is not empty then
        if(bucket.size() != 0){
            
            sapy::PDict to_union;
            sapy::PSet to_remove;

            //for each V-resolvent C of clauses in BV do
            for(auto clause1_it = bucket.begin(); clause1_it != bucket.end(); ++clause1_it){
                Clause clause1 = *clause1_it;
                for(auto clause2_it = (clause1_it); clause2_it != bucket.end(); ++clause2_it){
                    Clause clause2 = *clause2_it;
                    if(clause1 == clause2) continue;

                    bool got = false;
                    Clause v_resolvent_C = _resolve(clause1, clause2, variable, got);

                    if (got && v_resolvent_C.size() == 0) {
                        return Result::UNSAT;
                    }
                    if(got){
                        // U = first variable of clause C according to order π
                        Variable U = _firstVariable(v_resolvent_C);

                        to_remove.add(clause1);
                        to_remove.add(clause2);

                        // Bu = Bu ∪ {C}
                        Clause C;
                        C.add(v_resolvent_C);
                        if(!to_union.contain(U)){
                            to_union[U] = Clause();
                        }

                        to_union[U] = to_union[U].unwrap<Clauses>().union_(C);
                        // goto next;
                    }

                    //Buckets[U] = Buckets[U].unwrap<Clauses>().union_(C);
                }
                
            }
// next:
            Clauses updated_bucket;
            for (const auto& clause : bucket) {
                if (!to_remove.contain(clause)) {
                    updated_bucket.add(clause);
                }
            }
            Buckets[variable] = updated_bucket;
            LOG_DETAIL("to_union: {}", to_union.toString().toStdString());
            for(auto it = to_union.begin(); it != to_union.end(); it++){
                Variable U = it->first;
                Clauses new_clauses = it->second.unwrap<Clauses>();
                Buckets[U] = Buckets[U].unwrap<Clauses>().union_(new_clauses);
            }

            LOG_DETAIL("Buckets: {}", Buckets.toString().toStdString());
            
        }
    }

    return Result::SAT;
}

Result Solver::solve(Method method){
    // Formula form = Formula(formula_);
    // _condition(form, "3");

    switch(method){
        case DP:
            return _solve_DP(formula_);
        case DPLL:
            return Result::ERROR;
        case RESOLUTION:
            return _solve_Resolution(formula_,sapy::PSet());
        case CDCL:
            return Result::ERROR;
    }
    return Result::ERROR;
}

void Solver::readCNF(const sapy::PString _filename){
    std::string filename = _filename.toStdString();
    if(filename.empty()){
        LOG_ERROR("Filename is empty");
        return;
    }
    std::ifstream file(filename);
    if(!file){
        LOG_ERROR("File not found: {}", filename);
        return;
    }
    formula_.clear();
    std::string line;
    while(std::getline(file, line)){
        if (line.empty() || line[0] == 'c') {
            continue;
        }else if(line[0] == 'p'){
            sapy::PString line_ = line;
            sapy::PString val_cnt_str = line_.split(" ")[2];
            val_cnt_ = std::stoul(val_cnt_str.toStdString());
            continue;
        }
        sapy::PList tokens;
        sapy::PString token;
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                if (!(token == "")) {
                    tokens.append(token);
                    token = "";
                }
            }
            else {
                token += line[i];
            }
        }
        if (!(token == "")) {
            tokens.append(token);
        }

    
        Clause clause;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i].unwrap<sapy::PString>() == "0") {
                break;
            }

            sapy::PString token = tokens[i];
            bool negated = (token[0] == '-');
            sapy::PString varName = negated ? token.substr(1) : token;
            Literal literal = negated ? "¬" + varName : varName;
            clause.add(literal);
        }
        formula_.add(clause);
    }
}


}

