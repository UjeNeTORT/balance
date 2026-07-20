#ifndef MIR_PASSES_DCE_H
#define MIR_PASSES_DCE_H

#include "Pass.h"

#include <map>
#include <string>
#include <unordered_set>

namespace Balance {

class MachineFunction;

class DeadCodeElimination : public Pass {
    // temporary measure until we have MachineRegisterInfo
    struct RegInfo {

        Register Reg;
        // instructions that are used by the instruction
        // which defines this register
        std::unordered_set<const MachineInst *> SSAUses;
        // instructions that take this register as input
        std::unordered_set<const MachineInst *> SSAUsers;

        explicit RegInfo(Register Reg) : Reg(Reg) {}

        bool operator==(const RegInfo &other) const {
            return Reg == other.Reg;
        }
    };

    std::map<Register, RegInfo> MRI;

public:
    DeadCodeElimination(const std::string &Name = "dce") : Pass(Name) {}

    bool run(MachineFunction &MF) override;

    void rebuildMRI(MachineFunction &MF);
    void removeDef(MachineFunction &MF, Register Reg);
};

} // namespace Balance

#endif // MIR_PASSES_DCE_H
