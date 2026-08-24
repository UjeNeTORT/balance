#include "BasicBlock.h"
#include <algorithm>
#include <unordered_set>

using namespace Balance;

void BasicBlock::verify() const {
    if (Instructions.empty())
        return;

    bool IsTerminal = false;
    for (const auto& Instr: Instructions) {
        Instr.verify();
        if (IsTerminal)
            throwVerifyError("Terminal instruction not in the end of basic block");
        IsTerminal = Instr.isTerminal();

        if (Instr.getOpcode() == Opcodes::BR) {
            if (Instr.getBrDstBB().size() != Successors.size() ||
                !std::is_permutation(Successors.begin(), Successors.end(),
                                     Instr.getBrDstBB().begin())) {
                throwVerifyError("Successors != BR destinations");
            }
        } else if (Instr.getOpcode() == Opcodes::RET) {
            if (Successors.size() != 0)
                throwVerifyError("BB which ends with RET must have no successors");
        }
    }
    if (!IsTerminal)
        throwVerifyError("No terminal instruction in the end of basic block");

    std::unordered_set<const BasicBlock*> SuccSeen;
    for (const auto* Succ: Successors)
        if (!SuccSeen.insert(Succ).second)
            throwVerifyError("Successors list has duplicates");

    std::unordered_set<const BasicBlock*> PredSeen;
    for (const auto* Pred: Predecessors)
        if (!PredSeen.insert(Pred).second)
            throwVerifyError("Predecessors list has duplicates");

    for (const auto* Succ: Successors)
        if (std::find(Succ->getPredecessors().begin(), Succ->getPredecessors().end(), this) ==
                                                                    Succ->getPredecessors().end())
            throwVerifyError("Predecessor <=> Successor is broken (<= not found)");

    for (const auto* Pred: Predecessors)
        if (std::find(Pred->getSuccessors().begin(), Pred->getSuccessors().end(), this) ==
                                                                    Pred->getSuccessors().end())
            throwVerifyError("Predecessor <=> Successor is broken (=> not found)");
}

