#pragma once
#include "solver.hpp"
#include "DPLL.hpp"
#include <vector>

namespace ssat {

class DP_Solver final : public Solver {
public:
  std::string name() const override { return "DP_Solver"; }

  Result solve(const Formula& f, const SolveOptions& opt) override {
    // TODO: 
    (void)f; (void)opt;
    return Result::UNKNOWN;
  }

  void setFormula(const Formula& f) override {
    // TODO: Implement setFormula logic
    (void)f;
  }

  const std::vector<int8_t>& model() override { return model_; }
private:
  std::vector<int8_t> model_;
};

class CDCL_Solver final : public Solver {
public:
  std::string name() const override { return "CDCL_Solver"; }

  Result solve(const Formula& f, const SolveOptions& opt) override {
    // TODO:
    (void)f; (void)opt;
    return Result::UNKNOWN;
  }

  void setFormula(const Formula& f) override {
    // TODO: Implement setFormula logic
    (void)f;
  }
  const std::vector<int8_t>& model() override { return model_; }
private:
  std::vector<int8_t> model_;
};



}