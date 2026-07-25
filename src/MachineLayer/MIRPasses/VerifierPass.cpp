#include "VerifierPass.h"

#include "MIROpcodes.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineOperand.h"
#include "MachineInst.h"
#include "Register.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace Balance;

bool VerifierPass::run(MachineFunction &MF) {
    std::string Msg;

    auto RunAndHandleFail = [&Msg, this](bool Cond) -> bool {
        if (Cond) {
            this->reportFail(std::cerr, Msg);
            Msg.clear();
            return true;
        }
        return false;
    };

    bool Fail = false;
    Fail |= RunAndHandleFail(verifyCFG(MF, Msg));
    Fail |= RunAndHandleFail(verifyMBB(MF, Msg));
    Fail |= RunAndHandleFail(verifyMIDefsUses(MF, Msg));
    Fail |= RunAndHandleFail(verifySingleEntryMBB(MF, Msg));

    // TODO: in release mode this pass should be disabled

    if (Fail) reportFailAndTerminate(std::cerr, "Verification failed, cannot proceed (exiting...)\n");
    else reportSuccess(std::cerr);

    // pass does not change anything
    return false;
}

bool VerifierPass::verifyCFG(MachineFunction &MF, std::string &Msg) const {
    assert(Msg.empty());

    bool Fail = false;
    for (const MachineBB &MBB : MF) {
        if (MBB.getSuccessors().empty() && MBB.getPredecessors().empty() && !MF.getBasicBlocks().empty()) {
            Msg += std::string(MBB.getReferenceName())
                +  ": isolated basic block\n";
            Fail = true;
        };

        for (const MachineBB *Succ : MBB.getSuccessors()) {
            if (Succ == nullptr) {
                Msg += std::string(MBB.getReferenceName())
                    + ": nullptr in successors\n";
                Fail = true;
                continue;
            }
            if (std::find(Succ->pred_begin(), Succ->pred_end(), &MBB) == Succ->pred_end()) {
                Msg += std::string(MBB.getReferenceName())
                    + " -> "
                    + std::string(Succ->getReferenceName())
                    + ": Successor does not know its predecessor\n";
                Fail = true;
            }
        }
        for (const MachineBB *Pred : MBB.getPredecessors()) {
            if (Pred == nullptr) {
                Msg += std::string(MBB.getReferenceName())
                    + ": nullptr in predecessors\n";
                Fail = true;
                continue;
            }
            if (std::find(Pred->succ_begin(), Pred->succ_end(), &MBB) == Pred->succ_end()) {
                Msg += std::string(Pred->getReferenceName())
                + " -> "
                + std::string(MBB.getReferenceName())
                + ": Predecessor does not know its successor\n";
                Fail = true;
            }
        }
    }
    return Fail;
}
bool VerifierPass::verifyMBB(MachineFunction &MF, std::string &Msg) const {
    assert(Msg.empty());

    bool Fail = false;
    for (const MachineBB &MBB : MF) {
        for (const MachineInst &MI : MBB) {
            if (MI.getParent() != &MBB) {
                Msg += "\"" + MI.getAsmString() + "\" does not point to " + std::string(MBB.getReferenceName()) + "\n";
                Fail = true;
            }
        }
    }
    return Fail;
}

bool VerifierPass::verifyMIDefsUses(MachineFunction &MF, std::string &Msg) const {
    assert(Msg.empty());

    bool Fail = false;
    for (const MachineBB &MBB : MF) {
        for (const MachineInst &MI : MBB) {
            unsigned NumDefs = 0;
            for (const auto &MO : MI.getOperands()) {
                if (!MO.isReg()) continue;
                if (MO.isDef()) {
                    NumDefs++;
                    if (MO.isUse()) {
                        Msg += "\"" + MI.getAsmString() + "\": operand is both def and use: " + MO.getAsmString() + "\n";
                        Fail = true;
                    }
                }
            }

            unsigned ExpectedNumDefs = getNumDefs(MI.getOpcode());
            if (NumDefs != ExpectedNumDefs) {
                Msg += "\"" + MI.getAsmString() + "\": has " + std::to_string(NumDefs) + " defs, but expected " + std::to_string(ExpectedNumDefs) + "\n";
                Fail = true;
            }
        }
    }
    return Fail;
}

bool VerifierPass::verifySingleEntryMBB(MachineFunction &MF, std::string &Msg) const {
    assert(Msg.empty());

    bool Fail = false;
    unsigned NumEntryMBBs = 0;
    for (const MachineBB &MBB : MF) {
        if (MBB.getPredecessors().empty()) {
            NumEntryMBBs++;
        }
    }
    if (NumEntryMBBs != 1) {
        Msg += "Number of entry basic blocks is " + std::to_string(NumEntryMBBs) + ", expected 1\n";
        Fail = true;
    }
    return Fail;
}

void VerifierPass::reportSuccess(std::ostream &OS, const std::string &Msg) {
    OS << "["<< getName() << "] verification success\n";
    if (!Msg.empty())
        OS << Msg << '\n';
}

void VerifierPass::reportFail(std::ostream &OS, const std::string &Msg) {
    OS << "["<< getName() << "] verification failure\n";
    if (!Msg.empty())
        OS << Msg << '\n';
}

void VerifierPass::reportFailAndTerminate(std::ostream &OS, const std::string &Msg) {
    reportFail(OS, Msg);
    std::exit(1);
}
