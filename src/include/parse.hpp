#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include "formula.hpp"


namespace ssat {

struct DimacsParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct DimacsOptions {
    bool require_header = true;      // Require "p cnf ..." header
    bool strict_var_bound = false;   // Error if literal exceeds nVars from header
    bool allow_eof_without_0 = false;// Allow EOF without terminating 0 (non-standard)
};

Formula parse_dimacs(std::istream& in, const DimacsOptions& opt = {});

inline Formula parse_dimacs_file(const std::string& path, const DimacsOptions& opt = {}) {
    std::ifstream fin(path);
    if (!fin) throw DimacsParseError("DIMACS: cannot open file: " + path);
    return parse_dimacs(fin, opt);
}

} // namespace ssat
