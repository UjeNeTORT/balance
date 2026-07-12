#include "PhiElimination.h"
#include "MIROpcodes.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <unordered_map>

using namespace Balance;

bool PhiElimination::run(MachineFunction &MF) {
    std::vector<MachineBB *> OriginalMBBs;

    // use pointers because we will modify BBs
    for (auto &MBB : MF) {
        OriginalMBBs.push_back(&MBB);
    }

    CopyCandidatesT CopyCandidates; // includes swap candidates as well
    CopyCandidatesT SwapCandidates;

    for (auto *MBBPtr : OriginalMBBs) {
        auto &MBB = *MBBPtr;

        MachineBB::iterator InstPHI;
        do {
            InstPHI = std::find_if(MBB.begin(), MBB.end(), [](const MachineInst &MI) {
                return MI.getOpcode() == RISCVOpcode::PHI;
            });

            if (InstPHI == MBB.end()) continue;

            prepareEliminatePHI(MBB, *InstPHI, CopyCandidates);
            MBB.eraseMI(InstPHI);
        } while (InstPHI != MBB.end());
    }

    // avoid situation when two PHIs swap two regs:
    // R67 = PHI R0:MBB1, R1:MBB2
    // R0  = PHI R67:MBB1, R1:MBB2
    // ... eliminated into:
    // MOV R67, R0
    // MOV R0, R67
    for (auto &C1 : CopyCandidates) {
        for (auto &C2 : CopyCandidates) {
            if (C1 == C2) continue;
            if (std::find(SwapCandidates.begin(), SwapCandidates.end(), C1) != SwapCandidates.end()) continue;
            if (std::find(SwapCandidates.begin(), SwapCandidates.end(), C2) != SwapCandidates.end()) continue;
            if (std::get<0>(C1) == std::get<1>(C2) &&
                std::get<1>(C1) == std::get<0>(C2) &&
                std::get<2>(C1) == std::get<2>(C2)) {
                    SwapCandidates.push_back(C1);
            }
        }
    }

    for (auto &CC : CopyCandidates) {
        if (std::find(SwapCandidates.begin(), SwapCandidates.end(), CC) != SwapCandidates.end()) continue;
        createCopy(std::get<0>(CC), std::get<1>(CC), std::get<2>(CC));
    }

    for (auto &CC : SwapCandidates) {
        Register Tmp = MF.getNewVreg();
        createCopy(Tmp, std::get<1>(CC), std::get<2>(CC));
        createCopy(std::get<1>(CC), std::get<0>(CC), std::get<2>(CC));
        createCopy(std::get<0>(CC), Tmp, std::get<2>(CC));
    }
    return false;
}

// find all pairs Reg:MBB in operands, split critical edges when needed,
// push to Candidates pair consisting of Reg and MBB where copy is to be placed
bool PhiElimination::prepareEliminatePHI(MachineBB &MBB, MachineInst &PHI,
                                         PhiElimination::CopyCandidatesT &CopyCandidates) {
    assert(PHI.getOpcode() == RISCVOpcode::PHI && "eliminatePHI() called on non-PHI instruction");

    const auto &Defs = PHI.getDefs();
    assert(Defs.size() > 0);
    Register PHIDef = PHI.getDefs()[0];

    // pairs Reg:MBB in arguments of PHI inst
    std::unordered_map<Register, MachineBB *> PHIRegMBBs;
    for (auto MOIt = PHI.getOperands().begin(); MOIt != PHI.getOperands().end(); ++MOIt) {
        if (!MOIt->isReg()) continue;
        if (MOIt->isDef()) continue;
        PHIRegMBBs.insert({MOIt->getReg(), std::next(MOIt)->getMBB()});
    }

    for (auto &RegMBB : PHIRegMBBs) {
        auto &Reg = RegMBB.first;
        auto PredMBBPtr = RegMBB.second;
        assert(std::find(MBB.getPredecessors().begin(), MBB.getPredecessors().end(), PredMBBPtr) != MBB.getPredecessors().end()
            && "MBB is not a predecessor, but is in PHI node");

        if (isCriticalEdge(*PredMBBPtr, MBB)) {
            // after that we are sure that copy is going to be
            // inserted not on a critical edge
            PredMBBPtr = splitEdge(*PredMBBPtr, MBB);
        }

        CopyCandidates.push_back({PHIDef, Reg, PredMBBPtr});
    }
    return false;
}

bool PhiElimination::createCopy(Register To, Register From, MachineBB *MBB) {
    std::cerr << "[PhiElimination] creating copy: " << To << " <- " << From << " at " << MBB->getReferenceName() << "\n";

    // insertion point
    MachineBB::iterator TerminatorInst = std::prev(MBB->end());
    assert(isControlTransferInst(*TerminatorInst) && "Trying to insert into MBB without terminator");

    MBB->insertMI(TerminatorInst, RISCVOpcode::ADD)
        .addReg(To)
        .addReg(From)
        .addReg(Register(RISCV::RISCVRegister::ZERO));
    return false;
}
