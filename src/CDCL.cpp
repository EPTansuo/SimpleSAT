#include "CDCL.hpp"

#include <cassert>

namespace ssat {

bool CDCL_Solver::enqueue(Lit p, int from_cid) {
    int v = var(p);
    int8_t want = sign(p) ? int8_t(-1) : int8_t(+1); // make literal p == true
    if (asg_[v] != 0) {
        return asg_[v] == want;
    }
    asg_[v] = want;
    level_[v] = decisionLevel();
    reason_[v] = from_cid;
    phase_[v] = want;         // phase saving
    trail_.push_back(p);
    return true;
}

int CDCL_Solver::propagate() {
    while (qhead_ < trail_.size()) {
        Lit p = trail_[qhead_++];

        // clauses watching ~p may become unit/conflict
        Lit lfalse = neg(p);
        auto &ws = watches_[litIndex(lfalse)];

        for (size_t i = 0; i < ws.size(); ) {
            int cid = ws[i];
            Clause &c = clauses_[cid];
            assert(c.size() >= 2);

            // ensure c[0] is the watched literal equal to lfalse (if not, swap with c[1])
            if (c[0].x != lfalse.x) std::swap(c[0], c[1]);
            assert(c[0].x == lfalse.x);

            Lit other = c[1];
            int8_t other_val = litValue(other);
            if (other_val == 1) {
                // satisfied
                ++i;
                continue;
            }

            // try to find a new watch among c[2..] that is not false
            size_t k = 2;
            for (; k < c.size(); ++k) {
                if (litValue(c[k]) != -1) break;
            }

            if (k < c.size()) {
                // move watch: c[0] (lfalse) -> c[k]
                Lit new_watch = c[k];
                c[k] = c[0];
                c[0] = new_watch;

                // remove cid from current watch list by swap-pop
                ws[i] = ws.back();
                ws.pop_back();

                // add cid to new watch list
                watches_[litIndex(new_watch)].push_back(cid);
                continue; // do not i++
            }

            // no replacement watch => unit or conflict on 'other'
            if (other_val == -1) {
                return cid; // conflict clause
            }
            if (!enqueue(other, cid)) {
                return cid; // conflict by opposite assignment
            }
            ++i;
        }
    }
    return -1;
}

void CDCL_Solver::cancelUntil(int lvl) {
    if (decisionLevel() <= lvl) return;

    // keep assignments up to the start of decision level (lvl+1)
    // trail_lim_[0] is the boundary between level-0 and level-1
    int target_trail_sz = trail_lim_[lvl];

    for (int i = (int)trail_.size() - 1; i >= target_trail_sz; --i) {
        Lit p = trail_[i];
        int v = var(p);
        asg_[v] = 0;
        level_[v] = 0;
        reason_[v] = -1;
    }

    trail_.resize(target_trail_sz);
    trail_lim_.resize(lvl);
    if (qhead_ > trail_.size()) qhead_ = trail_.size();
}


bool CDCL_Solver::allAssigned() const {
    for (int v = 0; v < (int)asg_.size(); ++v)
        if (asg_[v] == 0) return false;
    return true;
}

Var CDCL_Solver::pickBranchVar() const {
    Var best = -1;
    double best_act = -1.0;

    for (Var v = 0; v < (Var)asg_.size(); ++v) {
        if (asg_[v] != 0) continue;
        double a = activity_[v];
        if (a > best_act) {
            best_act = a;
            best = v;
        }
    }
    // if all 0 activities, best becomes first unassigned (still fine)
    if (best == -1) {
        for (Var v = 0; v < (Var)asg_.size(); ++v)
            if (asg_[v] == 0) return v;
    }
    return best;
}

void CDCL_Solver::varBumpActivity(Var v) {
    activity_[v] += var_inc_;
    if (activity_[v] > 1e100) {
        // rescale
        for (auto &x : activity_) x *= 1e-100;
        var_inc_ *= 1e-100;
    }
}

void CDCL_Solver::varDecayActivity() {
    // classical VSIDS: increase var_inc each conflict
    var_inc_ /= var_decay_;
}

int CDCL_Solver::addClause(Clause c, bool learnt) {
    if (c.empty()) {
        // empty clause => immediate UNSAT, but we handle by conflict at level 0
        clauses_.push_back(std::move(c));
        is_learnt_.push_back(learnt ? 1 : 0);
        return (int)clauses_.size() - 1;
    }
    if (c.size() == 1) {
        // make it watcher-friendly: duplicate
        c.push_back(c[0]);
    }
    clauses_.push_back(std::move(c));
    is_learnt_.push_back(learnt ? 1 : 0);
    int cid = (int)clauses_.size() - 1;
    attachClause(cid);
    return cid;
}

void CDCL_Solver::attachClause(int cid) {
    Clause &c = clauses_[cid];
    assert(c.size() >= 2);
    watches_[litIndex(c[0])].push_back(cid);
    watches_[litIndex(c[1])].push_back(cid);
}

void CDCL_Solver::analyze(int confl_cid, Clause& out_learnt, int& out_backtrack_lvl) {
    const int nVars = (int)asg_.size();
    std::vector<uint8_t> seen(nVars, 0);

    out_learnt.clear();
    out_learnt.push_back(mkLit(0, false)); // placeholder for UIP literal

    int pathC = 0;
    int idx = (int)trail_.size() - 1;

    Clause *c = &clauses_[confl_cid];
    Lit p = mkLit(0, false);

    // 1-UIP
    while (true) {
        // resolve with clause c
        for (Lit q : *c) {
            int v = var(q);
            if (seen[v]) continue;
            if (asg_[v] == 0) continue;

            if (level_[v] > 0) {
                seen[v] = 1;
                varBumpActivity(v);

                if (level_[v] == decisionLevel()) {
                    ++pathC;
                } else {
                    out_learnt.push_back(q);
                }
            }
        }

        // select last assigned literal at current decision level that is seen
        while (true) {
            p = trail_[idx--];
            if (seen[var(p)]) break;
        }

        int pv = var(p);
        seen[pv] = 0; // clear for possible minimization (not implemented)

        --pathC;
        if (pathC <= 0) break;

        int rcid = reason_[pv];

        if (rcid < 0) {
            // pv is a decision literal; it should only happen when pathC becomes 0 after decrement
            // If you hit here, your backtracking/levels/reasons are inconsistent.
            break;
        }

        c = &clauses_[rcid];
    }

    out_learnt[0] = neg(p); // UIP literal

    // compute backtrack level = max level among other literals
    int bt = 0;
    for (size_t i = 1; i < out_learnt.size(); ++i) {
        bt = std::max(bt, level_[var(out_learnt[i])]);
    }
    out_backtrack_lvl = bt;

    varDecayActivity();
}

void CDCL_Solver::buildModel() {
    model_ = asg_;
    for (auto &x : model_) {
        if (x == 0) x = +1; // arbitrary: set unassigned to true
    }
}

Result CDCL_Solver::solve(const Formula& f, const SolveOptions& opt) {
    (void)opt;
    formula_ = f;

    const CNF &cnf = formula_.getCNF();
    const int nVars = cnf.nVars();

    // init state
    clauses_.clear();
    is_learnt_.clear();

    watches_.assign(2 * std::max(0, nVars), {});
    asg_.assign(nVars, 0);
    level_.assign(nVars, 0);
    reason_.assign(nVars, -1);
    activity_.assign(nVars, 0.0);
    phase_.assign(nVars, +1);

    trail_.clear();
    trail_lim_.clear();
    qhead_ = 0;
    model_.clear();

    // add original clauses
    // also collect unit clauses to enqueue at level 0
    std::vector<std::pair<int, Lit>> root_units; // (cid, unit lit)

    for (const Clause &orig : cnf.clauses) {
        if (orig.empty()) {
            return Result::UNSAT;
        }
        Clause c = orig;
        bool is_unit = (c.size() == 1);

        int cid = addClause(std::move(c), /*learnt=*/false);
        if (is_unit) {
            // unit is clauses_[cid][0]
            root_units.push_back({cid, clauses_[cid][0]});
        }
    }

    // enqueue root unit clauses
    for (auto [cid, u] : root_units) {
        if (!enqueue(u, cid)) {
            return Result::UNSAT;
        }
    }

    // propagate at root
    {
        int confl = propagate();
        if (confl != -1) return Result::UNSAT;
    }

    // main CDCL loop
    while (true) {
        int confl = propagate();
        if (confl != -1) {
            // conflict
            if (decisionLevel() == 0) {
                return Result::UNSAT;
            }

            Clause learnt;
            int backtrack_lvl = 0;
            analyze(confl, learnt, backtrack_lvl);

            cancelUntil(backtrack_lvl);

            int learnt_cid = addClause(std::move(learnt), /*learnt=*/true);

            // assert learnt clause's UIP literal at backtrack level
            Lit assert_lit = clauses_[learnt_cid][0];
            if (!enqueue(assert_lit, learnt_cid)) {
                // should not happen, but keep safe
                return Result::UNSAT;
            }
            continue;
        }

        // no conflict
        if (allAssigned()) {
            buildModel();
            return Result::SAT;
        }

        // decision
        Var v = pickBranchVar();
        if (v < 0) {
            buildModel();
            return Result::SAT;
        }

        // start new decision level
        trail_lim_.push_back((int)trail_.size());

        // phase saving: phase_[v] is +1 => pick x (true), -1 => pick !x (false)
        Lit decision = mkLit(v, phase_[v] == -1);
        if (!enqueue(decision, -1)) {
            // shouldn't happen
            return Result::UNSAT;
        }
    }
}

} // namespace ssat
