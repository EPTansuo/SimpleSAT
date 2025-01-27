#include "solver.hpp"

namespace ssat{
Literal Solver::_complementry(const Literal& literal) const {
    if(literal.startswith("¬")){
        return literal.substr(1);
    }else{
        return "¬" + literal;
    }
}

Formula Solver::_condition(const Formula formula, const Literal& literal) const {
    LOG_DETAIL("Before Conditioning: {}, Literal: {}", formula.toString(),
                                                      literal.toString());
    Formula result;
    Literal complementary = _complementry(literal);
    for(auto it = formula.cbegin(); it != formula.cend(); it++){
        Clause clause = *it;
        if(clause.contain(literal)){
            //result.add(clause - literal);
        }else if(clause.contain(complementary)){
            result.add(clause - complementary);
        }else{
            result.add(clause);
        }
    }
    LOG_DETAIL("After Conditioning: {}", result.toString());
    return result;
}

// void Solver::_condition(Formula& formula, const Literal& literal) const {
//     LOG_DEBUG("Before Conditioning: {}, Literal: {}", formula.toString(),
//                                                       literal.toString());    
//     Literal complementary = _complementry(literal);
//     sapy::PList to_add;
//     sapy::PList to_remove;
//     for (auto it = formula.begin(); it != formula.end(); it++) {
//         Clause clause = *it;
//         if (clause.contain(literal)) {
//             to_remove.append(clause);  
//         } else if (clause.contain(complementary)) {
//             to_remove.append(clause);
//             to_add.append(clause - complementary);
//         } else {
//             // Do nothing
//         }
//     }
//     for(auto it = to_remove.begin(); it != to_remove.end(); it++){
//         formula.discard(*it);
//     }

//     for(auto it = to_add.begin(); it != to_add.end(); it++){
//         formula.add(*it);
//     }
//     LOG_DEBUG("After Conditioning: {}", formula.toString());
// }


Clause Solver::_resolve(const Clause clause1, const Clause clause2, const Variable& variable, bool& got) const{
    got = false;
    auto resolvent = Clause();
    LOG_DETAIL("Resolve: Clause1: {}, Clause2: {} with Variable: {}", clause1.toString(),
                                                  clause2.toString(), variable);
    Literal complementary = _complementry(variable);
    if(clause1.contain(variable) && clause2.contain(complementary)){
        resolvent = (clause1-variable).union_(clause2-complementary);
        got = true;
        
    }else if(clause1.contain(complementary) && clause2.contain(variable)){
        resolvent = (clause1-complementary).union_(clause2-variable);
        got = true;
    }

    if(got){
        LOG_DETAIL("{}-Resolvent: {}", variable, resolvent.toString());
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
            LOG_DETAIL("Resolvent: {}, Literal: {}", resolvent.toString(),
                                                      literal.toString());
            return resolvent;
        }
    }
    literal = Literal();
    return Clause();
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



}