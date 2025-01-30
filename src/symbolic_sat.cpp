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
        Buckets[Variable(_indice2variable(i))] = sapy::PSet();  //create empty bucket Bv
    }


    // for each clause C of Δ do
    for(auto clause_wrap: formula){
        Clause clause = clause_wrap;
        Variable variable = _firstVariable(clause); //V = first variable of C according to order π

        // Bv = Bv ∪ {OBDD(C)}
        BDD bdd = _ROBDD(clause);
        sapy::PSet set = sapy::PSet({bdd});
        Buckets[variable] = Buckets[variable].unwrap<sapy::PSet>().union_( set );
    }

    // for each variable V of Δ according to order π do
    for(size_t i=1; i<=val_cnt_; i++){
        Variable variable = _indice2variable(i);
        Clauses bucket = Buckets[variable].unwrap<Clauses>();
        LOG_DETAIL("Variable: {}, Bucket: {}", variable, bucket.toString());

        // if BV is not empty then
        if(bucket.size() != 0){
            // ΓV = conjunction of all OBDDs in BV
            
        }
            
    }
    return Result::ERROR;
}


}