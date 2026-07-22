#include "DomFrontier.h"
#include "DomTree.h"
#include "MachineBB.h"
#include "MachineFunction.h"

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

    for (const BBTy &BB : F) DomFront.emplace(BB, NodeSetTy());
    for (const BBTy &BB : F) {
        if (BB.getPredecessors().size() < 2) continue;

        const auto &IDom = DT.getIDom(&BB);

        for (const BBTy *P : BB.getPredecessors()) {
            auto Runner = P;
            while (Runner != IDom) {
                DomFront[Runner].insert(P);
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

template class DomFrontier<MachineFunction, MachineBB>;

} // namespace Balance
