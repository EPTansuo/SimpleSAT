#pragma once 

#include "formula.hpp"
#include "result.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <log.hpp>

namespace ssat{


class Solver {
public: 
    Solver() {}
    Solver(const std::string filename): filename(filename) {readCNF(filename);}
    void readCNF(const std::string filename);
    Result solve();
    Formula getFormula() const {
        return formula;
    }
    std::string toString() const {
        return formula.toString();
    }
    friend std::ostream& operator<<(std::ostream& os, const Solver& solver){
        os << "Solver: " +  solver.toString();
        return os;
    }
private:    
    std::string filename;
    Formula formula;
    bool UP(Formula &);
    bool PLE(Formula &);
    Result _solve(Formula);
    Literal chooseLiteral(Formula &);
    Formula& setVariable(Formula&, Literal, bool);
};

Formula& Solver::setVariable(Formula& formula, Literal literal, bool value){
    // Remove the clause that the literal makes it true
    formula.getClauses().remove_if([&](Clause& c){
        for(auto& l : c.getLiterals()){
            if(l.getName() == literal.getName() && l.isNegated() == value){
                return true;
            }
        }
        return false;
    });


    // Remove the literal with the false result 
    for(auto& c : formula.getClauses()){
        c.getLiterals().remove_if([&](Literal& l){
            if(l.getName() == literal.getName() && l.isNegated() != value){
                return true;
            }
            return false;
        });
    }

    return formula;

}
Literal Solver::chooseLiteral(Formula &formula){
    for(auto& clause : formula.getClauses()){
        for(auto& literal : clause.getLiterals()){
            return literal;  // RETURN FIRST LITERAL  // TODO: 
        } 
    }
    return Literal();
}

bool Solver::UP(Formula &formula){
    bool found_unit_clause = false;
    Literal unit_literal;
    for(auto& clause : formula.getClauses()){
            if(clause.getLiterals().size() == 1){
                unit_literal = clause.getLiterals().front();
                found_unit_clause = true;
                break;
            }
        }

        if (!found_unit_clause) {
            return false;
        }


        unit_literal.setValue(!unit_literal.isNegated());
   

        formula.getClauses().remove_if([&]( Clause& c){
            for( auto& l : c.getLiterals()){
                if(l.getName() == unit_literal.getName() && l.isNegated() == unit_literal.isNegated()){
                    LOG_DEBUG("delete clause: {}", c.toString());
                    return true; 
                }
            }
            return false;
        });

        for(auto& clause : formula.getClauses()){
            clause.getLiterals().remove_if([&]( Literal& l){
                if(l.getName() == unit_literal.getName() && l.isNegated() != unit_literal.isNegated()){
                    LOG_DEBUG("delete literal: {}", l.toString());
                    return true; 
                }
                return false;
        });

            if(clause.getLiterals().empty()){
                LOG_DETAIL("Find empty clause", clause.toString());
                return false;
            }
        }
    LOG_DETAIL("UP: {}", formula.toString());
    return true;
}
    


bool Solver::PLE(Formula &formula){
     LOG_DETAIL("PLE start: {}", formula.toString());
    
    if(formula.empty()){
        return false;
    }


    bool found_pure_literal = false;
    // Clause& selected_clause = formula.getClauses().front();
    Clause * selected_clause = nullptr;

    for(auto& c : formula.getClauses()){
        found_pure_literal = false;
        for(auto& l : c.getLiterals()){
            bool found = false;
            for(auto& c2 : formula.getClauses()){
                for(auto& l2 : c2.getLiterals()){
                    if(l.getName() == l2.getName() && c != c2){
                        //LOG_DETAIL("FOUND: l: {}, l2: {}", l.toString(), l2.toString());
                        found = true;
                        break;
                    }
                }
                if(found){
                    break;
                }
            }
            if(!found){
                selected_clause = &c;
                LOG_DETAIL("Selected literal: {}", l.toString());
                LOG_DETAIL("Selected clause: {}", selected_clause->toString());
                found_pure_literal = true;
                break;
            }
        }
        if(found_pure_literal){
            formula.getClauses().remove_if([&](Clause& c){
                if(c == *selected_clause){
                    LOG_DEBUG("delete clause: {}", c.toString());
                    return true;
                }
                return false;
            });
            break;
        }
        
    }
    LOG_DETAIL("After PLE: {}", formula.toString());
    return found_pure_literal;
}

Result Solver::_solve(Formula formula){
    // LOG_DEBUG("Before UP: {}", formula.toString());
    // 
    while(UP(formula));
    // LOG_DEBUG("After UP: {}", formula.toString());


    if(!formula.empty()){
        for(auto clause: formula.getClauses()){
            if(clause.getLiterals().empty()){
                return Result::UNSAT;
            }
        }
    }
    
    while(PLE(formula));
    if(formula.empty()){
        return Result::SAT;
    }
    for(auto& clause : formula.getClauses()){
        if(clause.getLiterals().empty()){
            return Result::UNSAT;
        }
    }
    LOG_DEBUG("Formula Size: {}", formula.getClauses().size());

    Literal l = chooseLiteral(formula);

    Formula formula1 = formula;
    Formula formula2 = formula;
    setVariable(formula1, l, true);
    setVariable(formula2, l, false);

    return _solve(formula1) == Result::SAT ? Result::SAT : _solve(formula2);

}


Result Solver::solve(){
    if(formula.empty()){
        LOG_ERROR("Formula is empty");
        return Result::ERROR;
    }
    

    return _solve(formula);

    return Result::UNKNOWN; // TODO
}


void Solver::readCNF(const std::string filename){
    if(filename.empty()){
        std::cerr << "Filename is empty" << std::endl;
        return;
    }
    std::ifstream file(filename);
    if(!file){
        std::cerr << "File not found: " << filename << std::endl; 
        return;
    }
    std::string line;
    while(std::getline(file, line)){

        if (line.empty() || line[0] == 'c' || line[0] == 'p') {
            continue;
        }

        std::vector<std::string> tokens;
        std::string token;
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else {
                token += line[i];
            }
        }
        if (!token.empty()) {
            tokens.push_back(token); 
        }

    
        Clause clause;
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "0") {
                break;
            }

    
            bool negated = (tokens[i][0] == '-');
            std::string varName = negated ? tokens[i].substr(1) : tokens[i];

            clause.push(Literal(varName, negated));
        }

        this->formula.push(clause);
    }
}


}

