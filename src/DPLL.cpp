#include <DPLL.hpp>
#include <cstdint>
#include <vector>
#include <cmath>

namespace ssat {

namespace detail {

// 0 = unassigned, +1 = true, -1 = false
using Assign = std::vector<int8_t>;

static inline int8_t litValue(Lit p, const Assign& asg) {
    // retVal: 1: literal=true，-1: literal=false，0: unssigned
    int v = var(p);
    int8_t a = asg[v];
    if (a == 0) return 0;
    return sign(p) ? int8_t(-a) : a;
}

// luet literal p = true, if conflict, return false.
static inline bool assignLitTrue(Lit p, Assign& asg, std::vector<int>& trail) {
    int v = var(p);
    int8_t want = sign(p) ? int8_t(-1) : int8_t(+1); // p is negatibe literal => var=false can let p = true
    if (asg[v] == 0) {
        asg[v] = want;
        trail.push_back(v);
        return true;
    }
    return asg[v] == want;
}

static inline void undoTo(Assign& asg, std::vector<int>& trail, size_t sz) {
    while (trail.size() > sz) {
        int v = trail.back();
        trail.pop_back();
        asg[v] = 0;
    }
}

static inline bool clauseSatisfied(const Clause& c, const Assign& asg) {
    for (Lit p : c) if (litValue(p, asg) == 1) return true;
    return false;
}

// Unit Propagation + Pure Literal Elimination, if conflict, return false.
static bool propagate(const CNF& cnf, Assign& asg, std::vector<int>& trail) {
    const int n = cnf.nVars();
    bool changed = true;

    while (changed) {
        changed = false;

        // --- Unit propagation ---
        for (const Clause& c : cnf.clauses) {
            bool sat = false;
            Lit unit{};
            int unassigned = 0;

            for (Lit p : c) {
                int8_t v = litValue(p, asg);
                if (v == 1) { sat = true; break; }
                if (v == 0) { unit = p; ++unassigned; if (unassigned > 1) break; }
            }

            if (sat) continue;

            if (c.empty()) return false;
            if (unassigned == 0) return false;

            if (unassigned == 1) {
                if (!assignLitTrue(unit, asg, trail)) return false;
                changed = true;
            }
        }

        // --- Pure literal elimination ---
        std::vector<uint8_t> pol(n, 0);
 
        for (const Clause& c : cnf.clauses) {
            if (clauseSatisfied(c, asg)) continue;
            for (Lit p : c) {
                int v = var(p);
                if (asg[v] != 0) continue;
                pol[v] |= sign(p) ? 2 : 1;
            }
        }

        for (int v = 0; v < n; ++v) {
            if (asg[v] != 0) continue;
            if (pol[v] == 1) {
                asg[v] = +1; trail.push_back(v); changed = true;
            } else if (pol[v] == 2) {
                asg[v] = -1; trail.push_back(v); changed = true;
            }
        }
    }

    return true;
}


struct BranchChoice {
    int var = -1;        // Variable index (0-based)
    int8_t first = +1;   // Assignment to try first：+1(true), -1(false)
};

static BranchChoice pickBranchJW(const CNF& cnf, const Assign& asg) {
    const int n = cnf.nVars();
    std::vector<double> pos(n, 0.0), neg(n, 0.0);

    for (const Clause& c : cnf.clauses) {
        if (clauseSatisfied(c, asg)) continue;

        const int k = (int)c.size();
        if (k <= 0) continue;
        const double w = std::ldexp(1.0, -k); // 2^{-k}

        for (Lit p : c) {
            int v = var(p);
            if (asg[v] != 0) continue;
            if (litValue(p, asg) != 0) continue;

            if (!sign(p)) pos[v] += w;  // x_v
            else          neg[v] += w;  // !x_v
        }
    }

    BranchChoice best;
    double bestScore = -1.0;

    for (int v = 0; v < n; ++v) {
        if (asg[v] != 0) continue;
        double sPos = pos[v], sNeg = neg[v];
        double s = (sPos > sNeg) ? sPos : sNeg;
        if (s > bestScore) {
            bestScore = s;
            best.var = v;
            best.first = (sPos >= sNeg) ? int8_t(+1) : int8_t(-1);
        }
    }
    return best;
}

static BranchChoice pickBranchDLIS(const CNF& cnf, const Assign& asg) {
    const int n = cnf.nVars();
    std::vector<int> pos(n, 0), neg(n, 0);

    for (const Clause& c : cnf.clauses) {
        if (clauseSatisfied(c, asg)) continue;
        for (Lit p : c) {
            int v = var(p);
            if (asg[v] != 0) continue;
            if (!sign(p)) pos[v]++; else neg[v]++;
        }
    }

    BranchChoice best;
    int bestCnt = -1;
    for (int v = 0; v < n; ++v) {
        if (asg[v] != 0) continue;
        if (pos[v] > bestCnt) { bestCnt = pos[v]; best = {v, +1}; }
        if (neg[v] > bestCnt) { bestCnt = neg[v]; best = {v, -1}; }
    }
    return best;
}

static BranchChoice pickBranchVar(const CNF& cnf, const Assign& asg) {
    const int n = cnf.nVars();
    std::vector<int> score(n, 0);

    for (const Clause& c : cnf.clauses) {
        if (clauseSatisfied(c, asg)) continue;
        for (Lit p : c) {
            int v = var(p);
            if (asg[v] == 0) score[v]++;
        }
    }

    int best = -1, bestScore = -1;
    for (int v = 0; v < n; ++v) {
        if (asg[v] != 0) continue;
        if (score[v] > bestScore) { bestScore = score[v]; best = v; }
    }
    return {best, +1};
}

static bool dpll(const CNF& cnf, Assign& asg, std::vector<int>& trail) {
    const size_t base = trail.size();

    if (!propagate(cnf, asg, trail)) {
        undoTo(asg, trail, base);
        return false;
    }

    
    BranchChoice br = pickBranchDLIS(cnf, asg);
    if (br.var < 0) return true;

    for (int8_t val : { br.first, int8_t(-br.first) }) {
        size_t t0 = trail.size();
        asg[br.var] = val;
        trail.push_back(br.var);

        if (dpll(cnf, asg, trail)) return true;

        undoTo(asg, trail, t0);
    }

    undoTo(asg, trail, base);
    return false;
}

} // namespace detail

Result DPLL_Solver::solve(const Formula& f, const SolveOptions& opt) {
    (void)opt;
    const CNF& cnf = f.getCNF();
    detail::Assign asg(cnf.nVars(), 0);
    std::vector<int> trail;
    trail.reserve(cnf.nVars());

    bool sat = detail::dpll(cnf, asg, trail);
    model_ = std::move(asg);

    return sat ? Result::SAT : Result::UNSAT;
}


} // namespace ssat
