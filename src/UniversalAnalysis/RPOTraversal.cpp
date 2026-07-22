#include "RPOTraversal.h"

#include "MachineLayer/MachineBB.h"
#include "MachineLayer/MachineFunction.h"
#include "IR/BasicBlock.h"
#include "IR/Function.h"

#include <list>
#include <unordered_set>

namespace Balance {

template <typename BBTy, typename FuncTy>
void RPOTraversal<BBTy, FuncTy>::dfs(BBTy *Block, std::unordered_set<BBTy *> &Visited) {
    Visited.insert(Block);

    for (BBTy *Succ : Block->getSuccessors()) {
        if (Visited.find(Succ) == Visited.end()) {
            dfs(Succ, Visited);
        }
    }
    PO.push_back(Block);
}

template <typename BBTy, typename FuncTy>
RPOTraversal<BBTy, FuncTy>::RPOTraversal(FuncTy &Func) {
    compute(Func);
}

template <typename BBTy, typename FuncTy>
void RPOTraversal<BBTy, FuncTy>::compute(FuncTy &Func) {
    RPO.clear();
    PO.clear();
    typename std::unordered_set<BBTy *> Visited;

    if (Func.begin() != Func.end()) {

        BBTy &EntryBB = *Func.entryBB();
        dfs(&EntryBB, Visited);
    }

    RPO.insert(RPO.end(), PO.rbegin(), PO.rend());
}

template class RPOTraversal<MachineBB, MachineFunction>;
template class RPOTraversal<BasicBlock, Function>;

} // namespace Balance
