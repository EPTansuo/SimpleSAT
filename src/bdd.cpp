#include "solver.hpp"

namespace ssat{

// BDD: bdd[0]: variable name, bdd[1]: low_branch, bdd[2]: high_branch
// Based on the concepts from "Introduction to Binary Decision Diagrams" by Henrik Reif Andersen
BDD Solver::_OBDD(Clause clause) const{
    return _OBDD_r(BDD(), clause, 0);
}

BDD Solver::_OBDD_r(BDD bdd, Clause clause, int branch) const{
    if(clause.size() == 0){
        return {branch};
    }

    // use shannon expansion to build OBDD
    Variable variable = _firstVariable(clause);

    auto t0_clause = clause - variable;
    auto t1_clause = Clause();
    auto t0 = _OBDD_r(bdd, t0_clause, 0);
    auto t1 = _OBDD_r(bdd, t1_clause, 1);
    bdd.append(variable);
    bdd.append(t0);
    bdd.append(t1);

    return bdd;
}



void traverse_BDD_r(const BDD& bdd,sapy::PList& nodes_leafs) {
    LOG_DETAIL("Visit node: {}", bdd[0].toString());
    nodes_leafs.append(bdd);

    if(bdd.size() == 1) {
        return; // leaf node
    }
    
    traverse_BDD_r(bdd[1], nodes_leafs);    
    traverse_BDD_r(bdd[2], nodes_leafs);
}

sapy::PList traverse_BDD(const BDD& bdd){
    sapy::PList nodes_leafs;
    traverse_BDD_r(bdd, nodes_leafs);
    return nodes_leafs;
}


BDD reduce_OBDD(const BDD& bdd){
    sapy::PSet to_remove;
    auto nodes_leafs = traverse_BDD(bdd);

    for(auto nl_it1 = nodes_leafs.begin(); nl_it1 != nodes_leafs.end(); ++nl_it1){
        BDD nl1 = *nl_it1;
        for(auto nl_it2 = ++nl_it1; nl_it2 != nodes_leafs.end(); ++nl_it2){
            BDD nl2 = *nl_it2;
            if(nl1 == nl2){  // same node
                to_remove.add(nl1);
            }
        }
    }


    
}


}

