#include "include/log.hpp"
#include "solver.hpp"   
#include <map>
#include <cmath>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <unordered_set>
namespace ssat{



/********************************************************
 * 
 * Davis-Putnam-Logemann-Loveland Algorithm (Ref:
 * https://www.cnblogs.com/chesium/p/15982285.html, also 
 * see Algorithm  in Handbook of satisfiability, second edition)
 * 
*********************************************************/

class NeuroCoreHeuristic {
public:
    explicit NeuroCoreHeuristic(const std::string& model_path);

    // CNF: num_vars / clauses -> logits[num_vars]
    std::vector<float> compute_logits(
        int num_vars,
        const std::vector<std::vector<int>>& clauses);

private:
    Ort::Env env_;
    Ort::Session session_;
    Ort::MemoryInfo mem_info_;
    std::string input_name_;
    std::string output_name_;
};

NeuroCoreHeuristic::NeuroCoreHeuristic(const std::string& model_path)
    : env_(ORT_LOGGING_LEVEL_WARNING, "neurocore"),
      session_(nullptr),
      mem_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault))
{
    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(1);
    session_options.SetGraphOptimizationLevel(
        GraphOptimizationLevel::ORT_ENABLE_ALL);

    session_ = Ort::Session(env_, model_path.c_str(), session_options);

    Ort::AllocatorWithDefaultOptions allocator;
    input_name_  = session_.GetInputNameAllocated(0, allocator).get();
    output_name_ = session_.GetOutputNameAllocated(0, allocator).get();
}

std::vector<float> NeuroCoreHeuristic::compute_logits(
    int num_vars,
    const std::vector<std::vector<int>>& clauses)
{
    const int64_t num_clauses  = static_cast<int64_t>(clauses.size());
    const int64_t num_literals = static_cast<int64_t>(2 * num_vars);

    std::vector<float> A_data(num_clauses * num_literals, 0.0f);

    auto idx = [num_literals](int64_t r, int64_t c) {
        return r * num_literals + c;
    };

    for (int64_t j = 0; j < num_clauses; ++j) {
        for (int lit : clauses[j]) {
            int v = std::abs(lit) - 1;   // 0-based var index
            if (v < 0 || v >= num_vars) continue;

            int col = (lit > 0) ? (2 * v) : (2 * v + 1);
            A_data[idx(j, col)] = 1.0f;
        }
    }

    std::vector<int64_t> input_shape{ num_clauses, num_literals };
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        mem_info_,
        A_data.data(),
        A_data.size(),
        input_shape.data(),
        input_shape.size()
    );

    const char* input_names[]  = { input_name_.c_str() };
    const char* output_names[] = { output_name_.c_str() };

    auto output_tensors = session_.Run(
        Ort::RunOptions{nullptr},
        input_names, &input_tensor, 1,
        output_names, 1
    );

    float* out_data = output_tensors[0].GetTensorMutableData<float>();
    auto out_shape =
        output_tensors[0].GetTensorTypeAndShapeInfo().GetShape();

    if (out_shape.size() != 1) {
        throw std::runtime_error("Unexpected logits shape");
    }

    int64_t n_vars_out = out_shape[0];
    if (n_vars_out != num_vars) {
        throw std::runtime_error("logits length != num_vars");
    }

    return std::vector<float>(out_data, out_data + n_vars_out);
}


Variable _mostFrequentLiteral(const Formula& formula) {

    std::map<Variable, int> freq;


    for(auto it = formula.cbegin(); it != formula.cend(); it++){
        Clause clause = *it;
        for(auto jt = clause.cbegin(); jt != clause.cend(); jt++){
            Literal literal = *jt;
            freq[literal]++;
        }
    }
    
    Variable bestLit;
    int bestCount = -1;

    for (const auto& [lit, count] : freq) {
        if (count > bestCount) {
            bestCount = count;
            bestLit = lit;
        }
    }
    return bestLit;
}

Variable _pickLiteralJW(const Formula& formula) {
    // Jeroslow–Wang literal heuristic:
    // score(lit) = sum_{clause C containing lit} 2^{-|C|}

    std::map<Variable, double> score;

    for(auto it = formula.cbegin(); it != formula.cend(); it++){
        Clause clause = *it;
        if (clause.size() == 0) continue;
        double w = std::pow(2.0, -static_cast<int>(clause.size()));

        for (const Variable& lit : clause) {
            score[lit] += w;
        }
    }

    Variable bestLit;
    double bestScore = -1e100;
    bool found = false;

    for (const auto& [lit, s] : score) {
        if (!found || s > bestScore) {
            bestScore = s;
            bestLit = lit;
            found = true;
        }
    }

    
    return bestLit;
}

std::vector<std::vector<int>> formula_to_cnf(const Formula& formula) {
    std::vector<std::vector<int>> cnf;
    cnf.reserve(formula.size());

    for (auto it = formula.cbegin(); it != formula.cend(); it++) {
        Clause cl = *it;
        std::vector<int> clause;
        clause.reserve(cl.size());
        for (const Literal& lit : cl) {
            int v = std::stoi(lit.lstrip("¬"));
            int val = lit.startswith("¬") ? -v : v;
            clause.push_back(val);
        }
        cnf.push_back(std::move(clause));
    }
    return cnf;
}

static std::unique_ptr<NeuroCoreHeuristic> neurocore_;
static std::vector<int> neuro_order_; 

void Solver::_init_neuro_order(const Formula& formula) const {
    if (!neurocore_) {
        neurocore_ = std::make_unique<NeuroCoreHeuristic>("/home/han/minineurocore.onnx");
    }
    int num_vars = this->getValCnt();
    auto cnf = formula_to_cnf(formula);
    auto logits = neurocore_->compute_logits(num_vars, cnf);

    neuro_order_.resize(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        neuro_order_[i] = i + 1;
    }
    std::sort(neuro_order_.begin(), neuro_order_.end(),
    [&](int a, int b){
        return logits[a-1] > logits[b-1]; // logits 越大越靠前
    });
}

Variable Solver::_pickByNeuroCore(const Formula& formula) const {
    std::unordered_set<int> alive;
    for (auto it = formula.cbegin(); it != formula.cend(); it++) {
        Clause clause = *it;
        for (auto jt = clause.cbegin(); jt != clause.cend(); jt++) {
            Literal literal = *jt;
            int idx = _variable2indice(literal.lstrip("¬")); // 1..num_vars
            alive.insert(idx);
        }
    }

    for (int v : neuro_order_) {
        if (alive.count(v)) {
            return _indice2variable(v);
        }
    }

    // fallback，极端情况下（空 formula 之前不会走到这里）
    return _pickLiteralJW(formula);
}

Result Solver::_solve_DPLL_classic(Formula formula,int depth) const{
    if(depth == 0){
        _init_neuro_order(formula);
    }
    LOG_DEBUG("Formula: {}", formula.toString());
    formula = _unit_progate(formula);

    LOG_DEBUG("After UP: {}", formula.toString());

    if(formula.contains(Clause())){
        return Result::UNSAT;
    }

    formula = _pure_iteral_elimination(formula);
    LOG_DEBUG("After PLE: {}", formula.toString());

    if(formula.size()==0){
        return Result::SAT;
    }

    if(formula.contains(Clause())){
        return Result::UNSAT;
    }
    Variable variable;
    // Variable variable = _firstVariable(*formula.begin());
    // Variable variable = _mostFrequentLiteral(formula);
    // Variable variable = _pickLiteralJW(formula);
    // Variable variable = _pickByNeuroCore(formula);
    if (depth <= this->getValCnt()/5 && formula.size() > 20) {
    // if(false){
        //variable = _pickByNeuroCore(formula);
        variable = _firstVariable(*formula.begin());
    } else {
        variable = _pickLiteralJW(formula);  // fallback
    }

    Formula formula1 = _condition(formula, variable);
    Formula formula2 = _condition(formula, _complementry(variable));

    return _solve_DPLL_classic(formula1,depth+1) == Result::SAT ? Result::SAT : 
           _solve_DPLL_classic(formula2,depth+1);

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