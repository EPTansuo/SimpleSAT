#pragma once
#include "solver.hpp"

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
};

class DPLL_Solver final : public Solver {
public:
  std::string name() const override { return "DPLL_Solver"; }
  Result solve(const Formula& f, const SolveOptions& opt) override {
    (void)f; (void)opt;
    return Result::UNKNOWN;
  }

  void setFormula(const Formula& f) override {
    // TODO: Implement setFormula logic for DPLL_Solver
    (void)f;
  }
};

}