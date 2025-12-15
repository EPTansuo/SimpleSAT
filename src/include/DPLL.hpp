#pragma once

#include "formula.hpp"
#include "solver.hpp"

namespace ssat {

class DPLL_Solver final : public Solver {
public:
    std::string name() const override { return "DPLL_Solver"; }
    Result solve(const Formula& f, const SolveOptions& opt) override;
    void setFormula(const Formula& f) override { formula_ = f; }
    const std::vector<int8_t>& model() override { return model_; }
private:
    Formula formula_;
    std::vector<int8_t> model_;
};

} // namespace ssat
