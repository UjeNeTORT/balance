#include "RPOTraversal.h"
#include "MachineBB.h"

#include "MachineFunction.h"

#include <list>
#include <unordered_set>

using namespace Balance;

void RPOTraversal::dfs(MachineBB *MBB, std::unordered_set<MachineBB *> &Visited) {
    Visited.insert(MBB);

    for (MachineBB *Succ : MBB->getSuccessors()) {
        if (Visited.find(Succ) == Visited.end()) {
            dfs(Succ, Visited);
        }
    }
    PO.push_back(MBB);

    RPO.insert(RPO.end(), PO.rbegin(), PO.rend());
}

RPOTraversal::RPOTraversal(MachineFunction &MF) {
    compute(MF);
}

void RPOTraversal::compute(MachineFunction &MF) {
    RPO.clear();
    PO.clear();
    std::unordered_set<MachineBB *> Visited;

    if (MF.begin() != MF.end()) {
        dfs(&*MF.begin(), Visited);
    }
}

std::list<MachineBB *> RPOTraversal::getRPO() && { return std::move(RPO); }
std::list<MachineBB *> RPOTraversal::getPO() && { return std::move(PO); }

RPOTraversal::iterator RPOTraversal::begin() { return RPO.begin(); }
RPOTraversal::iterator RPOTraversal::end()   { return RPO.end(); }

RPOTraversal::const_iterator RPOTraversal::begin() const { return RPO.begin(); }
RPOTraversal::const_iterator RPOTraversal::end()   const { return RPO.end(); }
