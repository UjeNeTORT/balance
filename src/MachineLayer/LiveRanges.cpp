#include "LiveRanges.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineOperand.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <unordered_set>

namespace Balance {

std::unordered_set<Register> ComputeDefs(const MachineBB &MBB, bool ExcludePhi) {
    std::unordered_set<Register> Defs;

    for (const auto &MI : MBB) {
        if (ExcludePhi && MI.getOpcode() == PHI) continue;
        const auto &MIDefs = MI.getDefs();
        Defs.insert(MIDefs.begin(), MIDefs.end());
    }

    return Defs;
}

std::unordered_set<Register> ComputeUses(const MachineBB &MBB, bool ExcludePhi) {
    std::unordered_set<Register> Uses;

    for (const auto &MI : MBB) {
        if (ExcludePhi && MI.getOpcode() == PHI) continue;
        const auto &MIUses = MI.getUses();
        Uses.insert(MIUses.begin(), MIUses.end());
    }

    return Uses;
}

// use is upward-exposed in a bb when there is no local definition PRECEDING it,
// i.e. live range escapes the bb at the top
std::unordered_set<Register> ComputeUpwardExposed(const MachineBB &MBB, bool ExcludePhi) {
    std::unordered_set<Register> UE;

    // handle case when use comes before def (possible for physical regs)
    std::unordered_set<Register> RollingDefs;
    for (const auto &MI : MBB) {
        if (ExcludePhi && MI.getOpcode() == PHI) continue;
        const auto &MIUses = MI.getUses();
        const auto &MIDefs = MI.getDefs();

        for (const auto &Use : MIUses) {
            if (RollingDefs.find(Use) == RollingDefs.end()) {
                UE.insert(Use);
            }
        }

        RollingDefs.insert(MIDefs.begin(), MIDefs.end());
    }

    return UE;
}

std::unordered_set<Register> ComputePhiDefs(const MachineBB &MBB) {
    std::unordered_set<Register> PhiDefs;

    for (const auto &MI : MBB) {
        if (MI.getOpcode() != PHI) continue;
        const auto &MIDefs = MI.getDefs();
        PhiDefs.insert(MIDefs.begin(), MIDefs.end());
    }

    return PhiDefs;
}

std::unordered_set<Register> ComputePhiUses(const MachineBB &MBB) {
    std::unordered_set<Register> PhiUses;

    const auto &Successors = MBB.getSuccessors();
    for (const MachineBB *Succ : Successors) {
        for (const auto &MI : *Succ) {
            if (MI.getOpcode() != PHI) continue;
            const std::vector<MachineOperand> &MOs = MI.getOperands();

            for (auto It = ++MOs.begin(); It != MOs.end(); ++It) {
                if (It->isMBB()) continue; // skip MBB
                assert(std::next(It) != MOs.end() && "PHI should have an even number of non-def operands");
                const auto &MOReg = *It;
                const auto &MOMBB = *std::next(It);

                assert(MOReg.isReg() && MOMBB.isMBB() && "Wrong Reg:MBB pair in PHI");

                // if none of MOs come from this MBB, skip such PHI,
                // otherwise Register corresponding to this MBB will be added to PhiUses
                if (MOMBB.getMBB() != &MBB) continue;
                PhiUses.insert(MOReg.getReg());
            }
        }
    }

    return PhiUses;
}

// dataflow equation:
// LiveIns(B) = PhiDefs(B) U UpwardExposed(B) U (LiveOuts(B) \ Defs(B))
std::unordered_set<Register> ComputeLiveIns(const MachineBB &MBB) {
    std::unordered_set<Register> LiveIns;

    const auto &PhiDefsVec = ComputePhiDefs(MBB);
    const auto &UpwardExposedVec = ComputeUpwardExposed(MBB);
    const auto &LiveOutsVec = ComputeLiveOuts(MBB);
    const auto &DefsVec = ComputeDefs(MBB);

    auto InsertIfNotInDefsB = [&LiveIns, &DefsVec](Register R) {
        if (DefsVec.find(R) == DefsVec.end()) {
            LiveIns.insert(R);
        }
    };

    LiveIns.insert(PhiDefsVec.begin(), PhiDefsVec.end());
    LiveIns.insert(UpwardExposedVec.begin(), UpwardExposedVec.end());
    std::for_each(LiveOutsVec.begin(), LiveOutsVec.end(), InsertIfNotInDefsB);

    return LiveIns;
}

// dataflow equation:
// LiveOuts = for each direct successor S of B,
//  U [(LiveIns(S) \ PhiDefs(S)) U PhiUses(B)]
std::unordered_set<Register> ComputeLiveOuts(const MachineBB &B) {
    std::unordered_set<Register> LiveOuts;

    const auto &PhiUsesVec = ComputePhiUses(B);
    LiveOuts.insert(PhiUsesVec.begin(), PhiUsesVec.end());

    for (const MachineBB *Succ : B.getSuccessors()) {
        const MachineBB &S = *Succ;
        const auto &LiveInsVec = ComputeLiveIns(S);
        const auto &PhiDefsVec = ComputePhiDefs(S);

        auto InsertIfNotInPhiDefsS = [&LiveOuts, &PhiDefsVec](Register R) {
            if (PhiDefsVec.find(R) == PhiDefsVec.end()) {
                LiveOuts.insert(R);
            }
        };
        std::for_each(LiveInsVec.begin(), LiveInsVec.end(), InsertIfNotInPhiDefsS);
    }

    return LiveOuts;
}
} // namespace Balance
