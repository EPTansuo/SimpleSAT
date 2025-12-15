#pragma once
#include "methods.hpp"
#include "solver.hpp"
#include "egsolver.h"
#include <memory>
// include 各个 solver 的头文件


namespace ssat {

// 3) factory 生成
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