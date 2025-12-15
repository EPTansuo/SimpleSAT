#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include "formula.hpp"
#include <parse.hpp>


namespace ssat {

namespace detail {

inline bool read_next_token(std::istream& in, std::string& tok, bool& bol) {
    tok.clear();

    for (;;) {
        int ch = in.peek();
        if (ch == EOF) return false;

        // skip whitespace, track begin-of-line
        while (ch != EOF && std::isspace(static_cast<unsigned char>(ch))) {
            char c = static_cast<char>(in.get());
            if (c == '\n' || c == '\r') bol = true;  // CRLF
            ch = in.peek();
        }
        if (ch == EOF) return false;

        // comment line only if at beginning of line
        if (bol && ch == 'c') {
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            bol = true;
            continue;
        }

        // token starts
        bol = false;
        break;
    }

    // read token until whitespace
    while (true) {
        int ch = in.peek();
        if (ch == EOF) break;
        if (std::isspace(static_cast<unsigned char>(ch))) break;
        tok.push_back(static_cast<char>(in.get()));
    }
    return true;
}


inline int parse_int_token(const std::string& tok) {
    std::size_t idx = 0;
    int v = 0;
    try {
        v = std::stoi(tok, &idx, 10);
    } catch (...) {
        throw DimacsParseError("DIMACS: invalid integer token: '" + tok + "'");
    }
    if (idx != tok.size()) {
        throw DimacsParseError("DIMACS: invalid integer token: '" + tok + "'");
    }
    return v;
}

} // namespace detail

Formula parse_dimacs(std::istream& in, const DimacsOptions& opt) {
    CNF cnf;
    bool saw_header = false;
    int declared_vars = -1;
    int declared_clauses = -1;
    bool bol = true;

    Clause cur;
    int max_var_seen = -1;    // 0-based

    std::string tok;
    while (detail::read_next_token(in, tok, bol)) {
        if (tok == "p") {
            // header: p cnf <vars> <clauses>
            std::string fmt, svars, scls;
            if (!detail::read_next_token(in, fmt, bol) ||
                !detail::read_next_token(in, svars, bol) ||
                !detail::read_next_token(in, scls, bol)) {
                throw DimacsParseError("DIMACS: truncated header after 'p'");
            }
            if (fmt != "cnf") {
                throw DimacsParseError("DIMACS: expected 'p cnf', got 'p " + fmt + "'");
            }
            declared_vars = detail::parse_int_token(svars);
            declared_clauses = detail::parse_int_token(scls);
            if (declared_vars < 0 || declared_clauses < 0) {
                throw DimacsParseError("DIMACS: negative values in header");
            }
            saw_header = true;
            cnf.nVars_ = declared_vars;
            cnf.clauses.reserve(static_cast<size_t>(declared_clauses));
            continue;
        }

        
        int lit = detail::parse_int_token(tok);

        if (lit == 0) {
            cnf.clauses.push_back(cur);
            cur.clear();
            continue;
        }

        int v1 = std::abs(lit);
        if (v1 == 0) {
            throw DimacsParseError("DIMACS: literal 0 used inside clause");
        }

        if (saw_header && opt.strict_var_bound && v1 > declared_vars) {
            throw DimacsParseError("DIMACS: variable index " + std::to_string(v1) +
                                   " exceeds declared nVars " + std::to_string(declared_vars));
        }


        int v0 = v1 - 1;
        max_var_seen = std::max(max_var_seen, v0);

        cur.push_back(fromDimacs(lit));
    }

    // EOF handling: if the last clause is not terminated by 0
    if (!cur.empty()) {
        if (!opt.allow_eof_without_0) {
            throw DimacsParseError("DIMACS: file ended before clause terminator 0");
        }
        cnf.clauses.push_back(cur);
        cur.clear();
    }

    if (opt.require_header && !saw_header) {
        throw DimacsParseError("DIMACS: missing header line 'p cnf <vars> <clauses>'");
    }

    
    int used_nVars = (max_var_seen >= 0) ? (max_var_seen + 1) : 0;
    if (saw_header) cnf.nVars_ = std::max(cnf.nVars_, used_nVars);
    else cnf.nVars_ = used_nVars;

    return Formula(std::move(cnf));
}



} // namespace ssat
