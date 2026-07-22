#include "DomTree.h"

#include "MachineBB.h"
#include "MachineFunction.h"

#include <algorithm>
#include <cassert>

namespace Balance {

DomTree::DomTree(const MachineFunction &MF) {
    compute(MF);
}

void DomTree::compute(const MachineFunction &MF) {
    Worklist.clear();
    refillNodeSet(MF);

    // refill Dom[N] for each N in NodeSet
    auto InitNodeDom = [&](const MachineBB *MBB) {
        assert(Dom.find(MBB) == Dom.end());
        Dom.emplace(MBB, NodeSet);
    };

    Dom.clear();
    std::for_each(NodeSet.begin(), NodeSet.end(), InitNodeDom);

    const MachineBB *E = MF.entryBB();
    Worklist.insert(E);

    // algorithm: https://pages.cs.wisc.edu/~fischer/cs701.f07/lectures/Lecture20.pdf
    while (!Worklist.empty()) {
        const auto *Curr = *Worklist.begin();
        Worklist.erase(Worklist.begin());

        const auto &Preds = Curr->getPredecessors();
        NodeSetTy New = computeDomIntersection(Preds);
        New.insert(Curr);

        if (New != Dom[Curr]) {
            Dom[Curr] = New;
            for (const auto *Succ : Curr->getSuccessors()) {
                Worklist.insert(Succ);
            }
        }
    }
}

void DomTree::refillNodeSet(const MachineFunction &MF) {
    NodeSet.clear();
    for (const auto &MBB : MF) {
        NodeSet.insert(&MBB);
    }
}

DomTree::NodeSetTy
DomTree::computeDomIntersection(const std::list<MachineBB *> &Preds) const {
    NodeSetTy Result;

    if (Preds.empty()) return Result;

    Result = Dom.at(Preds[0]);

    for (size_t I = 1; I < Preds.size(); ++I) {
        const auto &PredecessorDom = Dom.at(Preds[I]);
        NodeSetTy Intersection;
        std::set_intersection(Result.begin(), Result.end(),
                              PredecessorDom.begin(), PredecessorDom.end(),
                              std::inserter(Intersection, Intersection.begin()));
        Result = Intersection;
    }

    return Result;
}


bool DomTree::dominates(const MachineBB *MBBA, const MachineBB *MBBB) {
}

bool DomTree::dominates(const MachineInst *MIA, const MachineInst *MIB) {
}

void DomTree::print(std::ostream &OS) const {

}


} // namespace Balance
