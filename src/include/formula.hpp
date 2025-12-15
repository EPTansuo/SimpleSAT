#pragma once
#include <vector>
#include <string>
#include <ostream>
#include <algorithm>
#include <cstdlib>

namespace ssat {

using Var = int;

struct Lit {
    int x; // x = (var<<1) | sign, sign: 0=pos, 1=neg
};

// ---- MiniSat-style helpers ----
inline Lit mkLit(Var v, bool s = false) { return Lit{ (v << 1) | int(s) }; }
inline Var var(Lit p) { return p.x >> 1; }
inline bool sign(Lit p) { return (p.x & 1) != 0; }
inline Lit neg(Lit p) { return Lit{ p.x ^ 1 }; }
inline int toDimacs(Lit p) {
    int v = var(p) + 1;
    return sign(p) ? -v : v;
}
inline Lit fromDimacs(int lit) {
    int v = std::abs(lit) - 1;
    bool s = (lit < 0);
    return mkLit(v, s);
}

using Clause = std::vector<Lit>;

struct CNF {
    int nVars_ = 0;
    std::vector<Clause> clauses;
    int nVars() const { return nVars_; }
    int nClauses() const { return static_cast<int>(clauses.size()); }
};

class Formula {
public:
    Formula() = default;
    Formula(const Formula&) = default;
    Formula& operator=(const Formula&) = default;

    explicit Formula(CNF cnf) : cnf_(std::move(cnf)) {}

    CNF& getCNF() { return cnf_; }
    const CNF& getCNF() const { return cnf_; }

    int numVars() const { return cnf_.nVars(); }
    size_t numClauses() const { return cnf_.nClauses(); }

    void clear() {
        cnf_.clauses.clear();
        cnf_.nVars_ = 0;
    }

    // Add a clause (Lit-based)
    void add(const Clause& clause) {
        bumpVarCount(clause);
        cnf_.clauses.push_back(clause);
    }
    void add(Clause&& clause) {
        bumpVarCount(clause);
        cnf_.clauses.push_back(std::move(clause));
    }

    // Convenience: add a DIMACS-style clause (vector of ints, WITHOUT the trailing 0)
    void addDimacs(const std::vector<int>& lits) {
        Clause c;
        c.reserve(lits.size());
        for (int d : lits) c.push_back(fromDimacs(d));
        add(std::move(c));
    }

    std::string toString(bool one_based = true) const {
        std::string out;
        for (size_t i = 0; i < cnf_.clauses.size(); ++i) {
            const auto& clause = cnf_.clauses[i];
            out += "(";
            for (size_t j = 0; j < clause.size(); ++j) {
                Lit p = clause[j];
                Var v = var(p) + (one_based ? 1 : 0);
                if (sign(p)) out += "!";
                out += "x" + std::to_string(v);
                if (j + 1 < clause.size()) out += " | ";
            }
            out += ")";
            if (i + 1 < cnf_.clauses.size()) out += " & ";
        }
        return out;
    }

private:
    CNF cnf_;

    void bumpVarCount(const Clause& clause) {
        for (Lit p : clause) {
            cnf_.nVars_ = std::max(cnf_.nVars_, var(p) + 1);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Formula& f) {
        os << f.toString();
        return os;
    }
};

} // namespace ssat
