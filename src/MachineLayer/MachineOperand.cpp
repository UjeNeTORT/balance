#include "MachineOperand.h"
#include "MachineBB.h"

#include "Utils/Utils.h"

#include <cassert>
#include <iostream>

namespace Balance {

MachineOperand MachineOperand::createVReg(unsigned int RegId) {
    MachineOperand MO;
    MO.Type = MOType::VirtReg;
    MO.RegId = RegId;
    return MO;
}

MachineOperand MachineOperand::createImm(uint64_t Imm) {
    MachineOperand MO;
    MO.Type = MOType::Imm;
    MO.Imm = Imm;
    return MO;
}

MachineOperand MachineOperand::createMBB(MachineBB *MBB) {
    MachineOperand MO;
    MO.Type = MOType::MachineBB;
    MO.MBB = MBB;
    return MO;
}

void MachineOperand::print(std::ostream &OS) const {
    switch (Type) {
    case MOType::VirtReg:
        OS << "%VReg" << RegId;
        break;
    case MOType::PhysReg:
        OS << RegId; // TODO: getRegName(RegId) for physical
        break;
    case MOType::Imm:
        OS << Imm;
        break;
    case MOType::MachineBB:
        MBB->printReferenceName(OS);
        break;
    default:
        unreachable("Unexpected MOType");
        break;
    }
}

bool isVirtReg(const MachineOperand &MO) {
    return MO.getType() == MOType::VirtReg;
};

bool isDef(const MachineOperand &MO) {
    // the first MO is always a Def of a MI
    if (MO.getMI() == nullptr) return false;
    return MO.getMI()->getOperands()[0] == MO;
}

bool isUse(const MachineOperand &MO) {
    return !isDef(MO);
}

bool MachineOperand::operator==(const MachineOperand &MO2) const {
    if (Type != MO2.Type) return false;
    switch(Type) {
        case MOType::VirtReg:
            return RegId == MO2.RegId;
            break;
        case MOType::PhysReg:
            return RegId == MO2.RegId;
            break;
        case MOType::Imm:
            return Imm == MO2.Imm;
            break;
        case MOType::MachineBB:
            return MBB == MO2.MBB;
            break;
        default:
            unreachable("Unexpected MOType");
            break;
    };
}

bool MachineOperand::operator!=(const MachineOperand &MO2) const {
    return !(*this == MO2);
}

void operator<<(std::ostream &OS, const MachineOperand &MO) {
    MO.print(OS);
}

} // namespace Balance
