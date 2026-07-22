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
        assert(DomMap.find(MBB) == DomMap.end());
        DomMap.emplace(MBB, NodeSet);
    };

    DomMap.clear();
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

        if (New != DomMap[Curr]) {
            DomMap[Curr] = New;
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

    Result = DomMap.find(*Preds.begin())->second;

    for (auto It = std::next(Preds.begin()); It != Preds.end(); ++It) {
        const auto &PredecessorDom = DomMap.find(*It)->second;
        NodeSetTy Intersection;
        std::set_intersection(Result.begin(), Result.end(),
                              PredecessorDom.begin(), PredecessorDom.end(),
                              std::inserter(Intersection, Intersection.begin()));
        Result = Intersection;
    }

    return Result;
}


bool DomTree::dom(const MachineBB *MBBA, const MachineBB *MBBB) const {
    assert(DomMap.find(MBBA) != DomMap.end() && "Unknown Dom[MBBA]");
    assert(DomMap.find(MBBB) != DomMap.end() && "Unknown Dom[MBBB]");

    return DomMap.find(MBBB)->second.find(MBBA) != DomMap.find(MBBB)->second.end();
}

bool DomTree::dom(const MachineInst *MIA, const MachineInst *MIB) const {
    assert(DomMap.find(MIA->getMBB()) != DomMap.end() && "Unknown Dom[MIA->getMBB()]");
    assert(DomMap.find(MIB->getMBB()) != DomMap.end() && "Unknown Dom[MIB->getMBB()]");

    const MachineBB *ParentA = MIA->getMBB();
    const MachineBB *ParentB = MIB->getMBB();

    if (sdom(ParentA, ParentB)) return true;
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

bool DomTree::sdom(const MachineBB *MBBA, const MachineBB *MBBB) const {
    assert(DomMap.find(MBBA) != DomMap.end() && "Unknown Dom[MBBA]");
    assert(DomMap.find(MBBB) != DomMap.end() && "Unknown Dom[MBBB]");

    return dom(MBBA, MBBB) && MBBA != MBBB;
}

bool DomTree::sdom(const MachineInst *MIA, const MachineInst *MIB) const {
    const MachineBB *ParentA = MIA->getMBB();
    const MachineBB *ParentB = MIB->getMBB();

    return dom(ParentA, ParentB) && MIA != MIB;
}

bool DomTree::idom(const MachineBB *MBBA, const MachineBB *MBBB) const {
    const auto &Preds = MBBB->getPredecessors();
    return sdom(MBBA, MBBB) && std::find(Preds.begin(), Preds.end(), MBBA) != Preds.end();
}

void DomTree::print(std::ostream &OS) const {

}


} // namespace Balance
