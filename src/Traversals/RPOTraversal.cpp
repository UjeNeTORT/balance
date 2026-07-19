#include "RPOTraversal.h"

#include "MachineLayer/MachineBB.h"
#include "MachineLayer/MachineFunction.h"
#include "IR/BasicBlock.h"
#include "IR/Function.h"

#include <list>
#include <unordered_set>

namespace Balance {

template <typename BB, typename F>
void RPOTraversal<BB, F>::dfs(BB *Block, std::unordered_set<BB *> &Visited) {
    Visited.insert(Block);

    for (BB *Succ : Block->getSuccessors()) {
        if (Visited.find(Succ) == Visited.end()) {
            dfs(Succ, Visited);
        }
    }
    PO.push_back(Block);
}

template <typename BB, typename F>
RPOTraversal<BB, F>::RPOTraversal(F &Func) {
    compute(Func);
}

template <typename BB, typename F>
void RPOTraversal<BB, F>::compute(F &Func) {
    RPO.clear();
    PO.clear();
    typename std::unordered_set<BB *> Visited;

    if (Func.begin() != Func.end()) {

        BB &EntryBB = *Func.entryBB();
        dfs(&EntryBB, Visited);
    }

    RPO.insert(RPO.end(), PO.rbegin(), PO.rend());
}

template class RPOTraversal<MachineBB, MachineFunction>;
template class RPOTraversal<BasicBlock, Function>;

} // namespace Balance
