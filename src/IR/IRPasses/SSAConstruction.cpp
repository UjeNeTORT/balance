#include "SSAConstruction.h"

#include "IR/BasicBlock.h"
#include "IR/Function.h"
#include "IR/Instruction.h"
#include "IR/Operand.h"
#include "MachineLayer/MIROpcodes.h"
#include "UniversalAnalysis/DomFrontier.h"
#include "Utils.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

namespace Balance {

// returns mapping VirtReg v -> [Instructions where v is defined]
static std::unordered_map<VirtRegister, std::unordered_set<BasicBlock *>> getRegDefsBBs(Function &F) noexcept {
    std::unordered_map<VirtRegister, std::unordered_set<BasicBlock *>> Res;
    for (auto &BB : F) {
        for (auto &&I : BB) {
            for (auto &&Def : I.getDst()) {
                if (Res.find(Def) == Res.end()) {
                    Res.emplace(Def, std::unordered_set<BasicBlock *>{I.getParent()});
                } else {
                    assert(Res.find(Def) != Res.end() && "previous if should have checked this");
                    Res[Def].insert(I.getParent());
                }
            }
        }
    }

    return Res;
}

// insert phi node at the entry of basic block
static void insertPhi(BasicBlock *BB, VirtRegister Def, std::unordered_set<BasicBlock *> FromBBs) {
    BB->insertInstruction(BB->begin(), Opcodes::PHI)
        .addDst(Def)
        .addSrc(Def)
        .addSrc(Def);
}

bool SSAConstruction::run(Function &F) {
    auto &&DFInfo = DomFrontier<Function, BasicBlock>(F);
    auto &&RegToDefBBsMap = getRegDefsBBs(F);

    std::unordered_set<VirtRegister> Registers;
    for (auto [V, DefInsts] : RegToDefBBsMap)
        Registers.insert(V);

    for (const auto &V : Registers) {
        std::unordered_set<BasicBlock *> PhiAddedBBs; // basic blocks where phi nodes is added
        std::unordered_set<BasicBlock *> FromBBs;     // basic blocks containing definitions of v

        FromBBs = RegToDefBBsMap[V];

        while (!FromBBs.empty()) {
            const auto &Victim = FromBBs.begin();
            BasicBlock *X = *Victim;
            FromBBs.erase(Victim);

            for (BasicBlock *Y: DFInfo.getFrontier(X)) {
                if (!PhiAddedBBs.count(Y)) {
                    insertPhi(Y, V, FromBBs);
                    PhiAddedBBs.insert(Y);

                    if (!RegToDefBBsMap[V].count(Y)) {
                        FromBBs.insert(Y);
                    }
                }
            }
        }
    }

    return false;
}

} // namespace Balance


