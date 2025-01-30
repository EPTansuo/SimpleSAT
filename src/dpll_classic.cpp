#include "solver.hpp"   

namespace ssat{



/********************************************************
 * 
 * Davis-Putnam-Logemann-Loveland Algorithm (Ref:
 * https://www.cnblogs.com/chesium/p/15982285.html, also 
 * see Algorithm  in Handbook of satisfiability, second edition)
 * 
*********************************************************/



Result Solver::_solve_DPLL_classic(Formula formula) const{

    LOG_DEBUG("Formula: {}", formula.toString());
    formula = _unit_progate(formula);

    LOG_DEBUG("After UP: {}", formula.toString());

    if(formula.contain(Clause())){
        return Result::UNSAT;
    }

    formula = _pure_iteral_elimination(formula);
    LOG_DEBUG("After PLE: {}", formula.toString());

    if(formula.size()==0){
        return Result::SAT;
    }

    if(formula.contain(Clause())){
        return Result::UNSAT;
    }

    Variable variable = _firstVariable(*formula.begin());

    Formula formula1 = _condition(formula, variable);
    Formula formula2 = _condition(formula, _complementry(variable));

    return _solve_DPLL_classic(formula1) == Result::SAT ? Result::SAT : _solve_DPLL_classic(formula2);

}

Formula Solver::_unit_progate(Formula formula) const{
    bool found_unit_clause = false;
    Literal unit_literal;
    do{
        found_unit_clause = false;
        for(auto it = formula.cbegin(); it != formula.cend(); it++){
            Clause clause = *it;
            if(clause.size() == 1){
                found_unit_clause = true;
                unit_literal = *clause.begin();
                break;
            }
        }
        formula =  _condition(formula, unit_literal);
    }while(found_unit_clause);
    return formula;
}
Formula Solver::_pure_iteral_elimination(Formula formula) const{
    sapy::PList literals;
    sapy::PList pure_literals;

    // get all literals
    for(auto clause_wrap: formula){
        Clause clause = clause_wrap;
        for(auto literal_wrap: clause){
            Literal literal = literal_wrap;
            literals.append(literal);
        }
    }

    // get all pure literals
    for(auto it: literals){
        Literal complementary = _complementry(it);
        if(literals.count(it) + literals.count(complementary) == 1){
            if(literals.count(it) == 1){
                pure_literals.append(it);
            }else{
                pure_literals.append(complementary);
            }
        }
    }

    LOG_DEBUG("Pure Literals: {}", pure_literals.toString());
    // remove all clauses containing pure literals
    sapy::PList to_remove;
    for(auto it = formula.begin(); it != formula.end(); it++){
        Clause clause = *it;
        bool found = false;
        for(auto literal_wrap: clause){
            Literal literal = literal_wrap;
            if(pure_literals.count(literal) > 0){
                found = true;
                break;
            }
        }
        if(found){
            to_remove.append(clause);
        }
    }
    for(auto to_remove_item: to_remove){
        formula.discard(to_remove_item);
    }

    return formula;
}

} // namespace ssat