#pragma once

#include "formula.hpp"
#include "solver.hpp"

namespace ssat {

/********************************************************
 * 
 * DPLL Algorithm (Ref: Algorithm 2 in Chapter 4 of the
 * Handbook of satisfiability, second edition)
 * 
********************************************************/

class CDCL_Solver final : public Solver {
public:
    std::string name() const override { return "CDCL_Solver"; }
    Result solve(const Formula& f, const SolveOptions& opt) override{return Result::UNKNOWN;};
    void setFormula(const Formula& f) override { formula_ = f; }
    const std::vector<int8_t>& model() override { return model_; }
private:
    Formula formula_;
    std::vector<int8_t> model_;
};

} // namespace ssat
