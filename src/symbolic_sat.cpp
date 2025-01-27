#include "solver.hpp"

/********************************************************
* 
* SYMBOLIC_SAT Algorithm (Ref: Algorithm 3.2 in Handbook
* of satisfiability, second edition)
* 
********************************************************/


namespace ssat{

Result Solver::_solve_SYMBOLIC_SAT(Formula formula) const{

    sapy::PDict Buckets;
    // for each variable V of Δ do
    for(size_t i=1; i<=val_cnt_; i++){
        Buckets[Variable(std::to_string(i))] = Clauses();  //create empty bucket Bv
    }


    // for each clause C of Δ do
    for(auto clause_wrap: formula){
        Clause clause = clause_wrap;
        Variable variable = _firstVariable(clause); //V = first variable of C according to order π

        // Bv = Bv ∪ {OBDD(C)}
        
        // Buckets[variable] = Buckets[variable].unwrap<Clauses>().union_( xxxx );
    }


    return Result::ERROR;
}


}