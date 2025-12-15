#pragma once 

#include "formula.hpp"
#include "result.hpp"

namespace ssat{

struct SolveOptions {
    int verbosity = 0;
};

class Solver {
public: 
    virtual ~Solver() = default;
    virtual std::string name() const = 0;
    virtual Result solve(const Formula&f, const SolveOptions& opt={}) = 0;
    virtual void setFormula(const Formula& f) = 0;
};


} // namespace ssat