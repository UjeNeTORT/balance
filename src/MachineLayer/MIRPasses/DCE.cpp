#include "DCE.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "RISCV/RISCVRegisters.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <map>
#include <unordered_set>

namespace Balance {

void DeadCodeElimination::rebuildMRI(MachineFunction &MF) {
    MRI.clear();

    // just filling reginfo...
    for (const auto &MBB : MF) {
        for (const auto &MI : MBB) {
            for (const auto &MO : MI) {
                if (!MO.isReg()) continue;
                const Register Reg = MO.getReg();
                if (isReservedRegister(Reg)) continue;
                if (MRI.find(Reg) == MRI.end())
                    MRI.emplace(Reg, RegInfo(Reg));

                if (MO.isUse()) {
                    assert(MRI.find(Reg) != MRI.end());
                    MRI.find(Reg)->second.SSAUsers.insert(&MI);
                } else {
                    // example:
                    // Reg (def) = INSTRUCTION Use1, Use2
                    // here we look for instructions which define Use1, Use2
                    for (const auto &MO2 : MI) {
                        if (MO2 == MO) continue;
                        if (!MO2.isReg()) continue;
                        if (MO2.isDef()) continue;
                        const Register &UseN = MO2.getReg();
                        if (isReservedRegister(UseN)) continue;

                        for (const auto &MBB_ : MF) {
                            for (const auto &UseMI : MBB_) {
                                const auto &Defs = UseMI.getDefs();
                                if (std::find(Defs.begin(), Defs.end(), UseN) != Defs.end()) {
                                    MRI.find(Reg)->second.SSAUses.insert(&UseMI);
                                }
                            }
                        }

                    }
                }

            }
        }
    }
}

bool DeadCodeElimination::run(MachineFunction &MF) {
    bool Changed = false;

    for (auto &MBB : MF) {
        for (auto MIIt = MBB.begin(); MIIt != MBB.end(); /*nothing*/) {
            auto &MI = *MIIt;
            auto Defs = MI.getDefs();
            // if instruction defines only zero registers and has no side effects,
            // it is considered dead
            if (Defs.size() == 1 && Defs[0] == RISCV::RISCVRegister::ZERO && !MI.hasSideEffects()) {
                dbg() << "erasing (only def is RZ) " << MI << "\n";
                MIIt = MI.eraseFromParent();
                Changed = true;
            } else {
                ++MIIt;
            }
        }
    }

    rebuildMRI(MF); // TODO: implement real thing

    std::map<Register, int> VCnt; // ssa value <-> use count

    // initialize the map
    for (const auto &Pair : MRI) {
        const Register &V = Pair.first;
        const RegInfo &RI = Pair.second;
        VCnt.emplace(V, 0);
        for (const auto &U : RI.SSAUsers) {
            VCnt[V]++;
        }
    }

    // while entries with use count == 0 exist...
    while (std::count_if(VCnt.begin(), VCnt.end(), [](auto &P) { return P.second == 0; }) != 0) {
        std::map<Register, int> VZeroCnt;
        std::copy_if(VCnt.begin(), VCnt.end(), std::inserter(VZeroCnt, VZeroCnt.end()), [](auto &P) { return P.second == 0; });

        for (auto &Pair : VZeroCnt) {
            const Register &V = Pair.first;
            if (isReservedRegister(V)) continue;

            assert(MRI.find(V) != MRI.end());
            const RegInfo &RI = MRI.find(V)->second;

            for (auto &U : RI.SSAUses) {
                for (auto &RegU : U->getDefs()) {
                    if (isReservedRegister(RegU)) continue;
                    VCnt[RegU]--;
                }
            }

            VCnt.erase(V);
            removeDef(MF, V);
            rebuildMRI(MF);
            Changed = true;
        }
    }

    return Changed;
}

void DeadCodeElimination::removeDef(MachineFunction &MF, Register Reg) {
    for (auto &MBB : MF) {
        for (auto MIIt = MBB.begin(); MIIt != MBB.end(); /*nothing*/) {
            auto &MI = *MIIt;
            const auto &Defs = MI.getDefs();
            if (std::find(Defs.begin(), Defs.end(), Reg) != Defs.end()) {
                // at this point we decide to get rid of this Reg def and uses
                if (Defs.size() == 1) {
                    if (Defs[0] == Reg) {
                        dbg() << "erasing " << MI << "\n";
                        MIIt = MBB.eraseMI(MI.getIterator());
                        continue;
                    }
                } else if (Defs.size() > 1) {
                    auto It = std::find(MI.begin(), MI.end(), Reg);
                    dbg() << "marking def as ZERO in " << MI << "\n";
                    *It = Register(RISCV::RISCVRegister::ZERO);
                }
            }

            ++MIIt;
        }
    }
}

} // namespace Balance
