#include "MachineInst.h"
#include "MachineOperand.h"
#include "MIROpcodes.h"

#include <algorithm>
#include <ostream>
#include <vector>

namespace Balance {

class Register;

MachineInst &MachineInst::addReg(Register Reg) {
    MachineOperand MO = MachineOperand::createVReg(Reg.getId());
    MO.setMI(this);

    // by default first operand is always a def
    // however it is not true for instructions like stores
    // which have no defs at all
    // this is why we define number of explicit defs for
    // each instruction and use it to determine where to set
    // border between defs and uses in Operands vector
    if (Operands.size() < getNumDefs(Opcode)) {
        MO.setIsDef();
    } else {
        MO.setIsUse();
    }

    Operands.push_back(MO);

    return *this;
}

MachineInst &MachineInst::addImm(uint64_t Imm) {
    return addMO(MachineOperand::createImm(Imm));
}

MachineInst &MachineInst::addMBB(MachineBB *MBB) {
    return addMO(MachineOperand::createMBB(MBB));
}

MachineInst &MachineInst::addMO(MachineOperand MO) {
    MO.setMI(this);
    Operands.push_back(MO);
    return *this;
}

std::vector<Register> MachineInst::getDefs() const {
    std::vector<Register> Defs;
    for (const auto &MO : Operands) {
        if ((MO.isVReg() || MO.isPhysReg()) && MO.isDef()) {
            Defs.push_back(Register(MO.getVReg()));
        }
    }
    return Defs;
}

std::vector<Register> MachineInst::getUses() const {
    std::vector<Register> Uses;
    for (const auto &MO : Operands) {
        if ((MO.isVReg() || MO.isPhysReg()) && MO.isUse()) {
            Uses.push_back(Register(MO.getVReg()));
        }
    }
    return Uses;
}

void MachineInst::print(std::ostream &OS) const {
    OS << getInstNameByOpcode(Opcode) << " ";
    bool First = true;

    std::for_each(Operands.begin(), Operands.end(),
        [&First, &OS](const MachineOperand &Op) {
        if ( First ) First = false; else OS << ", ";
        Op.print(OS);
        }
    );
}

std::ostream &operator<<(std::ostream &OS, const MachineInst &MI) {
    MI.print(OS);
    return OS;
}

} // namespace Balance
