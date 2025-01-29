
#include "solver.hpp"

namespace ssat{


/********************************************************
* 
* Davis-Putnam-Logemann-Loveland Algorithm (Ref: Algorithm
* 3.3 in Handbook of satisfiability, second edition)
* 
********************************************************/



Result Solver::_solve_DPLL(Formula formula) const{
    auto result = _solve_DPLL_r(formula,0);
    if(std::get_if<Result>(&result) != nullptr){
        return *std::get_if<Result>(&result);
    }
    return Result::SAT;
}
std::variant<Result, Clause> Solver::_solve_DPLL_r(Formula formula, size_t depth) const {
    //if Δ = {} then
    if (formula.size() == 0) {
        return Clause(); 
    } 
    // else if {} ∈ Δ then
    else if (formula.contain(Clause())) {
        return Result::UNSAT;
    }

    Literal positive_literal = _indice2variable(depth + 1);
    Literal negative_literal = "¬" + _indice2variable(depth + 1);

    // else if L =dpll-(Δ|Pd+1, d + 1) != unsatisfiable then
    auto result_true = _solve_DPLL_r(_condition(formula, positive_literal), depth + 1);
    if (auto L = std::get_if<Clause>(&result_true)) {
        sapy::PSet literals;
        literals.add(positive_literal);
        return L->union_(literals); // return L ∪ {Pd+1}
    }

    // else if L =dpll-(Δ|¬Pd+1, d + 1) = unsatisfiable then
    auto result_false = _solve_DPLL_r(_condition(formula, negative_literal), depth + 1);
    if (auto L = std::get_if<Clause>(&result_false)) {
        sapy::PSet literals;
        literals.add(negative_literal);
        return L->union_(literals);  // return L ∪ {¬Pd+1}
    }

    return Result::UNSAT;
}

}