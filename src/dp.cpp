#include "solver.hpp"


namespace ssat{
Result Solver::_solve_DP(Formula formula){
    LOG_DEBUG("Formula: {}", formula.toString().toStdString());
    if(formula.size() == 1 && formula.contain(Clause())){
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
            while(true){
                bool changed = false;
                to_union.clear();
                to_remove.clear();
                bucket = Buckets[variable].unwrap<Clauses>();
                for(auto clause1_it = bucket.begin(); clause1_it != bucket.end(); ++clause1_it){
                    //Clause clause1 = *clause1_it;
                    auto clause2_it = clause1_it;
                    for(++clause2_it; clause2_it != bucket.end(); ++clause2_it){
                        Clause clause1 = *clause1_it;
                        Clause clause2 = *clause2_it;
                        //if(clause1 == clause2) continue;

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
                            Clauses C;
                            C.add(v_resolvent_C);
                            if(!to_union.contain(U)){
                                to_union[U] = Clauses();
                            }
                            to_union[U] = to_union[U].unwrap<Clauses>().union_(C);
                        }
                    }
                    
                }
                
                if(to_remove.size() == 0){
                    break;
                }

                Clauses updated_bucket;
                for (const auto& clause : bucket) {
                    if (!to_remove.contain(clause)) {
                        updated_bucket.add(clause);
                    }
                }
                Buckets[variable] = updated_bucket;

                LOG_DETAIL("to_union: {}", to_union.toString().toStdString());
                
                // Bu = Bu ∪ {C}                
                for(auto it = to_union.begin(); it != to_union.end(); it++){
                    Variable U = it->first;
                    Clauses new_clauses = it->second.unwrap<Clauses>();
                    Buckets[U] = Buckets[U].unwrap<Clauses>().union_(new_clauses);
                }
                
            }
            
            LOG_DETAIL("Buckets: {}", Buckets.toString().toStdString());
            
        }
    }

    return Result::SAT;
}


}