#pragma once

#include "formula.hpp"
#include "solver.hpp"
#include <cstdint>

namespace ssat {

/********************************************************
 * 
 * DPLL Algorithm (Ref: Algorithm 2 in Chapter 4 of the
 * Handbook of satisfiability, second edition)
 * 
********************************************************/

class CDCL_Solver final : public Solver {
public:
    std::string name() const override { return "CDCL_Solver"; }

    Result solve(const Formula& f, const SolveOptions& opt = {}) override;
    void setFormula(const Formula& f) override { formula_ = f; }
    const std::vector<int8_t>& model() override { return model_; }

private:
    // ---- watched-literals ----
    // watches_[litIndex(l)] holds clause-ids that currently watch literal l
    std::vector<std::vector<int>> watches_;

    // ---- clause database ----
    std::vector<Clause> clauses_;      // original + learned
    std::vector<uint8_t> is_learnt_;   // 0/1 tag

    // ---- assignment state ----
    Assign asg_;                       // 0/+1/-1, indexed by Var
    std::vector<int> level_;           // decision level per Var (0=root), valid iff asg_[v]!=0
    std::vector<int> reason_;          // clause-id that implied v, -1 for decision/root free
    std::vector<Lit> trail_;           // assigned literals in order
    std::vector<int> trail_lim_;       // indices of trail_ where each decision level starts
    size_t qhead_ = 0;                 // propagation head in trail_

    // ---- heuristics (simple VSIDS + phase saving) ----
    std::vector<double> activity_;     // per Var
    double var_inc_   = 1.0;
    double var_decay_ = 0.95;          // bump decay
    std::vector<int8_t> phase_;        // preferred assignment for Var: +1 (true) / -1 (false)

    // ---- output model ----
    std::vector<int8_t> model_;

    // ---- problem ----
    Formula formula_;

private:
    // decision level
    int decisionLevel() const { return static_cast<int>(trail_lim_.size()); }

    static inline int litIndex(Lit p) { return p.x; }

    inline int8_t litValue(Lit p) const {
        int v = var(p);
        int8_t a = asg_[v];
        if (a == 0) return 0;
        return sign(p) ? int8_t(-a) : a;
    }

    bool enqueue(Lit p, int from_cid);
    int  propagate();                       // returns conflict clause-id, or -1
    void cancelUntil(int lvl);              // backtrack to lvl
    bool allAssigned() const;

    Var  pickBranchVar() const;
    void varBumpActivity(Var v);
    void varDecayActivity();

    int  addClause(Clause c, bool learnt);
    void attachClause(int cid);

    void analyze(int confl_cid, Clause& out_learnt, int& out_backtrack_lvl);
    void buildModel();
};


} // namespace ssat
