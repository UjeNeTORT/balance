#include "DomFrontier.h"
#include "DomTree.h"
#include "MachineBB.h"
#include "MachineFunction.h"

#include "IR/Function.h"
#include "IR/BasicBlock.h"

#include <algorithm>
#include <cassert>

namespace Balance {

template<typename FuncTy, typename BBTy>
DomFrontier<FuncTy, BBTy>::DomFrontier(FuncTy &F) {
    compute(F);
}

template<typename FuncTy, typename BBTy>
void DomFrontier<FuncTy, BBTy>::compute(const FuncTy &F) {
    DomFront.clear();

    using Dummy = int;
    auto DT = DomTree<FuncTy, BBTy, Dummy>(F);

    for (const BBTy &BB : F) DomFront.emplace(&BB, NodeSetTy());
    for (const BBTy &BB : F) {
        if (BB.getPredecessors().size() < 2) continue;

        const auto &IDom = DT.getIDom(&BB);

        // bb with multiple predecessors (BB) is a candidate for a dominance frontier of some previous basic block
        // Algorithm:
        // - we start at some of its predecessors (P) and climb up the dom tree (using idom relationship)
        // - for each bb on this way (Runner), it is true that they dominate P, but do not dominate successor of P (BB)
        // - which exactly means that BB is in dom frontier of Runner
        for (const BBTy *P : BB.getPredecessors()) {
            auto Runner = P;

            // note that situation where Runner == IDom where Runner = P
            // is possible when for example bb2 is a loop bb and has an edge inside itself
            // and bb1 is its predecessor, so preds(bb2): bb1, bb2
            // and bb1 idom bb2 = true
            while (Runner != IDom) {
                DomFront[Runner].insert(&BB);
                Runner = DT.getIDom(Runner);
            }
        }
    }
}

template<typename FuncTy, typename BBTy>
const typename DomFrontier<FuncTy, BBTy>::NodeSetTy &
DomFrontier<FuncTy, BBTy>::getFrontier(const BBTy *BB) {
    assert(DomFront.find(BB) != DomFront.end() && "Unknown DomFront[BB]");
    return DomFront.find(BB)->second;
}

template<typename FuncTy, typename BBTy>
const typename DomFrontier<FuncTy, BBTy>::NodeSetTy
DomFrontier<FuncTy, BBTy>::getFrontier(const NodeSetTy &BBSet) {
    assert(std::all_of(BBSet.begin(), BBSet.end(), [this](const BBTy *BB) {
        return DomFront.count(BB) > 0;
    }) && "Unknown DomFront[BB]");

    // S - set of cfg nodes
    // DF(S) = U(DF(n)) for each n in S
    NodeSetTy Result;
    std::for_each(BBSet.begin(), BBSet.end(), [&](const BBTy *BB) {
        auto&& DFn = getFrontier(BB);
        Result.insert(DFn.begin(), DFn.end());
    });
    return Result;
}

template<typename FuncTy, typename BBTy>
const typename DomFrontier<FuncTy, BBTy>::NodeSetTy
DomFrontier<FuncTy, BBTy>::getIteratedFrontier(const NodeSetTy &BBSet) {
    assert(std::all_of(BBSet.begin(), BBSet.end(), [this](const BBTy *BB) {
        return DomFront.count(BB) > 0;
    }) && "Unknown DomFront[BB]");

    // iterated DF: DF^(S)
    // DF_{1}(S) = DF(S)
    // DF_{i+1}(S) = DF(S U DF_{i}(S))
    // i.e. DF^(S) is a transitive closure of DF(S) graph
    NodeSetTy IteratedDF;
    NodeSetTy DFi = getFrontier(BBSet);

    bool Converged = false;
    while (!Converged) {

        IteratedDF = DFi;
        DFi.insert(BBSet.begin(), BBSet.end());
        DFi = getFrontier(DFi);

        if (IteratedDF == DFi)
            Converged = true;
    }

    return IteratedDF;
}

template class DomFrontier<MachineFunction, MachineBB>;
template class DomFrontier<Function, BasicBlock>;

} // namespace Balance
