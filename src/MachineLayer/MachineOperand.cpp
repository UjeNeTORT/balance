#include "MachineOperand.h"
#include "MachineBB.h"

#include "Utils/Utils.h"

#include <cassert>
#include <iostream>

using namespace Balance;

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
    using MOType = MachineOperand::MOType;

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

