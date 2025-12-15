#pragma once
#include "methods.hpp"
#include "solver.hpp"
#include <memory>
#include "CDCL.hpp"
#include "DPLL.hpp"
#include "DP.hpp"

namespace ssat {


#define FACTORY_CASE(method, solver) \
  case Method::method: return std::make_unique<solver>();

inline std::unique_ptr<Solver> make_solver(Method m) {
  switch (m) {
    SSAT_METHOD_TABLE(FACTORY_CASE)
    default: break;
  }
  throw std::runtime_error("make_solver: unknown method");
}

} // namespace ssat