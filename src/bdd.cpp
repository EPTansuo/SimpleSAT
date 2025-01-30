#include "solver.hpp"
#include <stack>

namespace ssat{

BDD _iterative2recursive_BDD(const BDD& bdd);
BDD _recursive2iterative_BDD(const BDD& bdd);
BDD Solver::_ROBDD(Clause clause) const {

    LOG_DETAIL("Clause: {}", clause.toString());
    BDD table_T, table_H;
    BDD bdd_r = _ROBDD_r(BDD(),table_T, table_H, clause, 0);
    LOG_DETAIL("OBDD_r: {}", bdd_r.toString());

    return _recursive2iterative_BDD(bdd_r);

    // BDD bdd;  
    // std::stack<BDDNode> stack;

    // if (bdd_r != BDD()) {
    //     stack.push(*bdd_r.begin());  
    // }

    // // convert the recursive OBDD to iterative OBDD
    // while (!stack.empty()) {
    //     BDDNode n = stack.top();
    //     stack.pop();

    //     BDDNodeVar nvar = n.second;
    //     LOG_DETAIL("BDD_r_item: {}", n.first.toString());

    //     auto low_branch = nvar[1].is<BDD>() ? (*nvar[1].unwrap<BDD>().begin()).first : nvar[1];
    //     auto high_branch = nvar[2].is<BDD>() ? (*nvar[2].unwrap<BDD>().begin()).first : nvar[2];
    //     BDDNodeVar tmp = sapy::PList({nvar[0], low_branch, high_branch});
    //     bdd[n.first] = tmp;  

    //     if (nvar[2].is<BDD>()) {
    //         stack.push(*nvar[2].unwrap<BDD>().begin());
    //     }
    //     if (nvar[1].is<BDD>()) {
    //         stack.push(*nvar[1].unwrap<BDD>().begin());
    //     }
    // }

    // LOG_DETAIL("OBDD: {}", bdd.toString());
    // return bdd;
}

/********************************************************
 * 
 * Make BDD node unique (Ref: MK Algorithm in "Introduction 
 * to Binary Decision Diagrams" by Henrik Reif Andersen. 
 * 
********************************************************/
BDDNode Solver::_mk_unique_BDD(BDD& table_T, BDD& table_H, BDDNodeVar nvar) const{

    auto l = nvar[1];
    auto h = nvar[2];
    auto var = _indice2variable(nvar[0]);

    // if l = h then return l
    if(l == h){
        return h;

    // else if member(H, i, l, h) then
    }else if(table_H.contain(nvar)){
        return table_H[nvar];
    }else{
        table_T[var] = nvar; // u <- add(T, i, l, h, u)
        table_H[nvar] = var; // insert(H, i, h, u)
    }

    LOG_DETAIL("Unique BDDNode: {}", nvar.toString());
    // return u
    return BDDNode({var, nvar});

}


BDD Solver::_ROBDD_r(BDD bdd, BDD& table_T, BDD& table_H,  Clause clause, int branch) const{
    if(clause.size() == 0){
        return BDD();
    }

    // use shannon expansion to build OBDD
    Variable variable = _firstVariable(clause);
    Literal complementry = _complementry(variable);

    bool is_positive = clause.contain(variable);

    auto t0_clause = is_positive ? clause - variable : Clause();
    auto t1_clause = !is_positive  ? clause - complementry : Clause();
    BDD t0 = _ROBDD_r(bdd, table_T, table_H, t0_clause, is_positive? 0 : 1);
    BDD t1 = _ROBDD_r(bdd, table_T, table_H, t1_clause, is_positive? 1 : 0);
    BDDNodeVar nodeVar = {_variable2indice(variable), 
                t0!=BDD() ? (sapy::PAnyWrapper)t0 : (sapy::PAnyWrapper)(is_positive? 0 : 1),
                t1!=BDD() ? (sapy::PAnyWrapper)t1 : (sapy::PAnyWrapper)(is_positive? 1 : 0)};
    
    auto node = _mk_unique_BDD(table_T, table_H, nodeVar);
    bdd[node.first] = node.second;
    return bdd;
}




BDD _apply_BDD_r(sapy::PDict& cached, const BDD&lhs, const BDD &rhs) {
    if(cached.contain(sapy::PList({lhs, rhs}))){
        return cached[sapy::PList({lhs, rhs})];
    }
    
    
}

BDD _recursive2iterative_BDD(const BDD& bdd) {
    BDD bdd_i;
    std::stack<BDDNode> stack;
    if (bdd != BDD()) {
        stack.push(*bdd.begin());  
    }

    // convert the recursive OBDD to iterative OBDD
    while (!stack.empty()) {
        BDDNode n = stack.top();
        stack.pop();

        BDDNodeVar nvar = n.second;
        LOG_DETAIL("BDD_r_item: {}", n.first.toString());

        auto low_branch = nvar[1].is<BDD>() ? (*nvar[1].unwrap<BDD>().begin()).first : nvar[1];
        auto high_branch = nvar[2].is<BDD>() ? (*nvar[2].unwrap<BDD>().begin()).first : nvar[2];
        BDDNodeVar tmp = sapy::PList({nvar[0], low_branch, high_branch});
        bdd_i[n.first] = tmp;  

        if (nvar[2].is<BDD>()) {
            stack.push(*nvar[2].unwrap<BDD>().begin());
        }
        if (nvar[1].is<BDD>()) {
            stack.push(*nvar[1].unwrap<BDD>().begin());
        }
    }

    LOG_DETAIL("OBDD: {}", bdd_i.toString());
    return bdd_i;
}

BDD recursize_insert_BDD(BDD bdd, const BDDNode& bddnode) {
    LOG_DETAIL("bdd: {}", bdd.toString());
    if (bdd == BDD()) {
        LOG_DETAIL("Insert: key: {}, value: {}", bddnode.first, bddnode.second.toString());
        return {{bddnode.first, bddnode.second}};  
    }

    Variable key = bdd.begin()->first;  
    BDDNodeVar nvar_old = bdd[key].unwrap<BDDNodeVar>();
    
    BDD sub_bdd;
    for (auto branch : {1, 2}) { 
        LOG_DETAIL("nvar_old[branch]: {}, bdnode.first: {}, branch: {}", nvar_old[branch].toString(), bddnode.first, branch);
        if (nvar_old[branch] == bddnode.first) { 
            BDD bdd_tmp;
            bdd_tmp[bddnode.first] = bddnode.second;

            
            if (nvar_old[branch].is<BDD>()) {  
                sub_bdd = nvar_old[branch].unwrap<BDD>();  
            } else {  
                sub_bdd = BDD();  
            }

            sub_bdd = recursize_insert_BDD(sub_bdd, bddnode);

        }else{
            if(nvar_old[branch].is<BDD>()){
                sub_bdd = nvar_old[branch].unwrap<BDD>();
                sub_bdd = recursize_insert_BDD(sub_bdd, bddnode); 
                LOG_DETAIL("else sub_bdd: {}, branch: {}", sub_bdd.toString(), branch);
    
            }else{
                return bdd;
            }
        }

        bdd[key] = sapy::PList({
            nvar_old[0],  
            branch == 1 ? (sapy::PAnyWrapper)sub_bdd : nvar_old[1],
            branch == 1 ? nvar_old[2] : (sapy::PAnyWrapper)sub_bdd,
        });
    }

    return bdd;
}


BDD _iterative2recursive_BDD(const BDD& bdd) {
    BDD bdd_r;
    sapy::PList nodes_sorted;
    for(BDDNode bddnode: bdd){
        nodes_sorted.append(bddnode);
    }
    nodes_sorted.sort([](const BDDNode& b1, const BDDNode& b2){
        return b1.first < b2.first;
    });

    for(BDDNode bddnode: nodes_sorted){
        Variable key = bddnode.first;
        BDDNodeVar value = bddnode.second;
        LOG_DEBUG("Insert: key: {}, value: {}", key, value.toString());
        LOG_DETAIL("before insertion: {}", bdd_r.toString());
        bdd_r = recursize_insert_BDD(bdd_r, BDDNode(key, value));
        LOG_DETAIL("after insertion: {}", bdd_r.toString());
    }
    return bdd_r;
}

BDD Solver::_apply_and_BDD(const BDD& lhs, const BDD& rhs) const{
    if (lhs == BDD()) return BDD(); 
    if (rhs == BDD()) return BDD();

    BDD result;

    sapy::PSet vars;
    for(const auto& [var1_wrap, _] : lhs){
        vars.add(var1_wrap);
    }
    for(const auto& [var2_wrap, _] : rhs){
        vars.add(var2_wrap);
    }



    for (Variable var : vars) {
        
        bool in_lhs = lhs.contain(var);
        bool in_rhs = rhs.contain(var);
        
        if (in_lhs && in_rhs) {  

        
            BDDNodeVar node1 = lhs[var];
            BDDNodeVar node2 = rhs[var];

            int index = node1[0]; 
            auto low1 = node1[1], high1 = node1[2];
            auto low2 = node2[1], high2 = node2[2];


            auto apply_and_branch = [&](const sapy::PAnyWrapper& branch1, const sapy::PAnyWrapper& branch2) -> sapy::PAnyWrapper {
                if (branch1.is<BDD>() && branch2.is<BDD>()) {
                    return _apply_and_BDD(branch1.unwrap<BDD>(), branch2.unwrap<BDD>());
                } else if (branch1.is<BDD>()) {
                    return (branch2.unwrap<int>() == 0) ? sapy::PAnyWrapper(BDD()) : branch1;
                } else if (branch2.is<BDD>()) {
                    return (branch1.unwrap<int>() == 0) ? sapy::PAnyWrapper(BDD()) : branch2;
                } else {
                    return (int)(branch1.unwrap<int>() && branch2.unwrap<int>());
                }
            };

            sapy::PAnyWrapper low_branch = apply_and_branch(low1, low2);
            sapy::PAnyWrapper high_branch = apply_and_branch(high1, high2);
            

            result[var] = sapy::PList({index, low_branch, high_branch});

        }else{
            if(in_lhs){
                result[var] = lhs[var];
            }else{
                result[var] = rhs[var];
            }
        }

    }



    return result;
}


// BDD Solver::_apply_and_BDD(const BDD& lhs, const BDD& rhs) const{
//     BDD lhs_copy = lhs;
//     sapy::PList rhs_sorted;
//     for(auto it = rhs.begin(); it != rhs.end(); it++){
//         rhs_sorted.append(*it);
//     }
    
//     rhs_sorted.sort(this->varibale_order_,[](const std::pair<sapy::PAnyWrapper,sapy::PAnyWrapper>& b){
//         return b.first;
//     });

//     // for(auto it=rhs_sorted.begin(); it != rhs_sorted.end(); it++){
//     //     std::pair<sapy::PAnyWrapper,sapy::PAnyWrapper> bddnode = *it;
//     //     BDDNodeVar nvar = bddnode.second;
//     //     for(auto branch: {1,2}){
//     //         if(nvar[1].is<int>()){
//     //             if(nvar[1].unwrap<int>() == 0){
                    
//     //             }else{
//     //                 // do nothing
//     //             }
//     //       }
//     //     }
//     // }

//     for(auto it = lhs.begin(); it != lhs.end(); it++){
//         BDDNodeVar nvar = it->second;
//         for(auto branch: {1,2}){
//             if(nvar[1].is<int>()){
//                 if(nvar[1].unwrap<int>() == 0){
//                     std::pair<sapy::PAnyWrapper,sapy::PAnyWrapper> rhs_node = rhs_sorted[0];
//                     Variable key = rhs_node.first;
//                     BDDNodeVar value = rhs_node.second;
//                     lhs_copy[it->first] = BDDNodeVar({nvar[0], key, nvar[2]});
//                 }else{
//                     // do nothing
//                 }
//           }
//         }
//     }


//     // TODO
//     return lhs_copy;
// }


}

