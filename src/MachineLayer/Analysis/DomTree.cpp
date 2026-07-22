#include "DomTree.h"

#include "MachineBB.h"
#include "MachineInst.h"
#include "MachineFunction.h"
#include "Utils/Utils.h"

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

    Result = Dom.at(*Preds.begin());

    for (auto It = std::next(Preds.begin()); It != Preds.end(); ++It) {
        const auto &PredecessorDom = Dom.at(*It);
        NodeSetTy Intersection;
        std::set_intersection(Result.begin(), Result.end(),
                              PredecessorDom.begin(), PredecessorDom.end(),
                              std::inserter(Intersection, Intersection.begin()));
        Result = Intersection;
    }

    return Result;
}


bool DomTree::dominates(const MachineBB *MBBA, const MachineBB *MBBB) const {
    assert(Dom.find(MBBA) != Dom.end() && "Unknown Dom[MBBA]");
    assert(Dom.find(MBBB) != Dom.end() && "Unknown Dom[MBBB]");

    return Dom.find(MBBB)->second.find(MBBA) != Dom.find(MBBB)->second.end();
}

bool DomTree::dominates(const MachineInst *MIA, const MachineInst *MIB) const {
    assert(Dom.find(MIA->getMBB()) != Dom.end() && "Unknown Dom[MIA->getMBB()]");
    assert(Dom.find(MIB->getMBB()) != Dom.end() && "Unknown Dom[MIB->getMBB()]");

    const MachineBB *ParentA = MIA->getMBB();
    const MachineBB *ParentB = MIB->getMBB();

    if (properlyDominates(ParentA, ParentB)) return true;
    if (ParentA == ParentB) {
        for (auto It = ParentA->begin(), Ie = ParentA->end(); It != Ie; ++It) {
            auto &MI = *It;
            // now look who is first in parent mbb
            // if A => then A dom B = true
            if (MI == *MIA) return true;
            if (MI == *MIB) return false;
        }

    }

    return false;
}

// true if dominates(...) and A != B
bool DomTree::properlyDominates(const MachineBB *MBBA, const MachineBB *MBBB) const {
    assert(Dom.find(MBBA) != Dom.end() && "Unknown Dom[MBBA]");
    assert(Dom.find(MBBB) != Dom.end() && "Unknown Dom[MBBB]");

    return dominates(MBBA, MBBB) && MBBA != MBBB;
}

// true if dominates(...) and A != B
bool DomTree::properlyDominates(const MachineInst *MIA, const MachineInst *MIB) const {
    const MachineBB *ParentA = MIA->getMBB();
    const MachineBB *ParentB = MIB->getMBB();

    return dominates(ParentA, ParentB) && MIA != MIB;
}

void DomTree::print(std::ostream &OS) const {

}


} // namespace Balance
