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
        if(clause.contains(literal)){
            //result.add(clause - literal);
        }else if(clause.contains(complementary)){
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
//         if (clause.contains(literal)) {
//             to_remove.append(clause);  
//         } else if (clause.contains(complementary)) {
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
    if(clause1.contains(variable) && clause2.contains(complementary)){
        resolvent = (clause1-variable).union_(clause2-complementary);
        got = true;
        
    }else if(clause1.contains(complementary) && clause2.contains(variable)){
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
        if(clause2.contains(complementary)){
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
    sapy::PList sorted;
    for(auto it = clause.cbegin(); it != clause.cend(); it++){
        Literal literal = *it;
        if(literal.startswith("¬")){
            unsorted.add(literal.substr(1));
        }else{
            unsorted.add(literal);
        }
    }
    for(auto it = unsorted.begin(); it != unsorted.end(); it++){
        sorted.append(it->unwrap<Variable>());
    }
    sorted.sort(varibale_order_);

    return sorted[0];
}

int Solver::_variable2indice(const Variable& variable)const {
    if(sorted_variables_.count(variable) == 0){
        std::runtime_error("Variable not found");
    }
    return sorted_variables_.index(variable) + 1;
};
Variable Solver::_indice2variable(int indice) const{
    if(indice < 1 || indice > sorted_variables_.size()){
        std::runtime_error("Invalid indice");
    }
    return sorted_variables_[indice-1];
}




Result Solver::solve(Method method){
    // Formula form = Formula(formula_);
    // Formula result = _condition(form, "3");
    // std::cout << "Result: " << result.toString() << std::endl;
    // result = _condition(form, "¬3");
    // std::cout << "Result: " << result.toString() << std::endl;

    Formula form = Formula(formula_);
    Clause clause1 = *form.begin();
    
    Clause clause2 = *(++form.begin());
    BDD bdd1 = _ROBDD(clause1);
    BDD bdd2 = _ROBDD(clause2);

    LOG_INFO("BDD1: {}", bdd1.toString());
    LOG_INFO("BDD2: {}", bdd2.toString());
    _show_BDD(bdd1,false);
    _show_BDD(bdd2,false);
    BDD bdd3 = _apply_and_BDD(bdd1, bdd2);
    LOG_INFO("BDD3: {}", bdd3.toString());
    _show_BDD(bdd3,true);


    switch(method){
        case DP:
            return _solve_DP(formula_);
        case DPLL:
            //return Result::ERROR;
            return _solve_DPLL(formula_);
        case DPLL_CLASSIC:
            return _solve_DPLL_classic(formula_);
        case RESOLUTION:
            return _solve_Resolution(formula_,sapy::PSet());
        case SYMBOLIC_SAT:
            return _solve_SYMBOLIC_SAT(formula_);
        case CDCL:
            return Result::ERROR;
    }
    return Result::ERROR;
}

void Solver::readCNF(const sapy::PString _filename){
    std::string filename = _filename;
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
    sapy::PSet variables;
    digit_variable_ = true;
    while(std::getline(file, line)){
        if (line.empty() || line[0] == 'c') {
            continue;
        }else if(line[0] == 'p'){
            sapy::PString line_ = line;
            sapy::PString val_cnt_str = line_.split(" ")[2];
            val_cnt_ = std::stoul(val_cnt_str);
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

            variables.add(varName);
            if(digit_variable_){
                digit_variable_ = varName.isdigit();
            }
        }
        formula_.add(clause);
    }
    for(auto var: variables){
        sorted_variables_.append(var);
    }
    sorted_variables_.sort(varibale_order_);
    LOG_INFO("sorted_variables_: {}", sorted_variables_.toString());
    LOG_INFO("digit_variable_: {}", digit_variable_);
}





}