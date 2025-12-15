#pragma once

#include "formula.hpp"
#include "solver.hpp"
#include <stdexcept>

namespace ssat {


/********************************************************
 * 
 * Davis-Putnam Algorithm (Ref: Algorithm 3.1 in Handbook
 * of satisfiability, second edition)
 * 
********************************************************/

class DP_Solver final : public Solver {
public:
    std::string name() const override { return "DP_Solver"; }
    Result solve(const Formula& f, const SolveOptions& opt) override {
        throw std::runtime_error("DP_Solver::solve() not implemented yet.");
    };
    void setFormula(const Formula& f) override { formula_ = f; }
    const std::vector<int8_t>& model() override { return model_; }
private:
    Formula formula_;
    std::vector<int8_t> model_;
};

} // namespace ssat
