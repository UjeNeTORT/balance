#include "DomTree.h"

#include "IR/BasicBlock.h"
#include "IR/Function.h"
#include "IR/Instruction.h"
#include "MachineBB.h"
#include "MachineInst.h"
#include "MachineFunction.h"
#include "Utils/Utils.h"

#include <algorithm>
#include <cassert>

namespace Balance {

template<typename FuncTy, typename BBTy, typename InstTy>
DomTree<FuncTy, BBTy, InstTy>::DomTree(const FuncTy &MF) {
    compute(MF);
}

template<typename FuncTy, typename BBTy, typename InstTy>
void DomTree<FuncTy, BBTy, InstTy>::compute(const FuncTy &MF) {
    Worklist.clear();
    refillNodeSet(MF);

    // refill Dom[N] for each N in NodeSet
    auto InitNodeDom = [&](const BBTy *MBB) {
        assert(DomMap.find(MBB) == DomMap.end());
        DomMap.emplace(MBB, NodeSet);
    };

    DomMap.clear();
    std::for_each(NodeSet.begin(), NodeSet.end(), InitNodeDom);

    const BBTy *E = MF.entryBB();
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

    IDomMap.clear();
    // compute immediate dominators
    for (const auto &Pair : DomMap) {
        const auto &MBB = *Pair.first;
        const auto &DomSet = Pair.second;

        unsigned DomSetSize = DomSet.size();

        for (const auto &Dom : DomSet) {
            // skip self, we need strict dominators
            if (Dom == &MBB) continue;

            // to determine that node Dom is an immediate dominator of node MBB
            // we use the property of idom that the number of strict dominators of Dom
            // must be less by 1 than number of strict dominators of MBB.
            // Since only difference matters, we use sizes of dominators sets instead
            // of strict dominators as they are always less by 1
            if (DomMap.find(Dom)->second.size() + 1 == DomSetSize) {
                assert(IDomMap.empty());
                IDomMap.insert({&MBB, Dom});
                break;
            }
        }
    }
}

template<typename FuncTy, typename BBTy, typename InstTy>
void DomTree<FuncTy, BBTy, InstTy>::refillNodeSet(const FuncTy &MF) {
    NodeSet.clear();
    for (const auto &MBB : MF) {
        NodeSet.insert(&MBB);
    }
}

template<typename FuncTy, typename BBTy, typename InstTy>
typename DomTree<FuncTy, BBTy, InstTy>::NodeSetTy
DomTree<FuncTy, BBTy, InstTy>::computeDomIntersection(const std::list<BBTy *> &Preds) const {
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


template<typename FuncTy, typename BBTy, typename InstTy>
bool DomTree<FuncTy, BBTy, InstTy>::dom(const BBTy *BBA, const BBTy *BBB) const {
    assert(DomMap.find(BBA) != DomMap.end() && "Unknown Dom[MBBA]");
    assert(DomMap.find(BBB) != DomMap.end() && "Unknown Dom[MBBB]");

    return DomMap.find(BBB)->second.find(BBA) != DomMap.find(BBB)->second.end();
}

template<typename FuncTy, typename BBTy, typename InstTy>
bool DomTree<FuncTy, BBTy, InstTy>::dom(const InstTy *IA, const InstTy *IB) const {
    assert(DomMap.find(IA->getParent()) != DomMap.end() && "Unknown Dom[IA->getParent()]");
    assert(DomMap.find(IB->getParent()) != DomMap.end() && "Unknown Dom[IB->getParent()]");

    const MachineBB *ParentA = IA->getParent();
    const MachineBB *ParentB = IB->getParent();

    if (sdom(ParentA, ParentB)) return true;
    if (ParentA == ParentB) {
        for (auto It = ParentA->begin(), Ie = ParentA->end(); It != Ie; ++It) {
            auto &MI = *It;
            // now look who is first in parent mbb
            // if A => then A dom B = true
            if (MI == *IA) return true;
            if (MI == *IB) return false;
        }

    }

    return false;
}

template<typename FuncTy, typename BBTy, typename InstTy>
bool DomTree<FuncTy, BBTy, InstTy>::sdom(const BBTy *BBA, const BBTy *BBB) const {
    assert(DomMap.find(BBA) != DomMap.end() && "Unknown Dom[BBA]");
    assert(DomMap.find(BBB) != DomMap.end() && "Unknown Dom[BBB]");

    return dom(BBA, BBB) && BBA != BBB;
}

template<typename FuncTy, typename BBTy, typename InstTy>
bool DomTree<FuncTy, BBTy, InstTy>::sdom(const InstTy *IA, const InstTy *IB) const {
    const MachineBB *ParentA = IA->getParent();
    const MachineBB *ParentB = IB->getParent();

    return dom(ParentA, ParentB) && IA != IB;
}

template<typename FuncTy, typename BBTy, typename InstTy>
bool DomTree<FuncTy, BBTy, InstTy>::idom(const BBTy *BBA, const BBTy *BBB) const {
    assert(IDomMap.find(BBB) != IDomMap.end() && "Unknown IDom[BBB]");
    return IDomMap.find(BBB)->second == BBA;
}

template<typename FuncTy, typename BBTy, typename InstTy>
typename DomTree<FuncTy, BBTy, InstTy>::NodeSetTy
DomTree<FuncTy, BBTy, InstTy>::getSDoms(const BBTy *BB) {
    assert(DomMap.find(BB) != DomMap.end() && "Unknown Dom[BB]");
    NodeSetTy SDoms = DomMap.find(BB)->second;
    SDoms.erase(BB);
    return SDoms;
}

template<typename FuncTy, typename BBTy, typename InstTy>
const BBTy *DomTree<FuncTy, BBTy, InstTy>::getIDom(const BBTy *BB) {
    assert(IDomMap.find(BB) != IDomMap.end() && "Unknown IDom[BB]");
    return IDomMap.find(BB)->second;
}

template<typename FuncTy, typename BBTy, typename InstTy>
void DomTree<FuncTy, BBTy, InstTy>::print(std::ostream &OS) const {
    (void)OS; // suppress warning
    unreachable("Unimplemented");
}

template class DomTree<MachineFunction, MachineBB, MachineInst>;

} // namespace Balance
