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

bool MachineOperand::isVReg() const {
    return Type == MOType::VirtReg;
}

bool MachineOperand::isPhysReg() const {
    return Type == MOType::PhysReg;
}

bool MachineOperand::isImm() const {
    return Type == MOType::Imm;
}

bool MachineOperand::isMBB() const {
    return Type == MOType::MachineBB;
}

unsigned MachineOperand::getVReg() const {
    assert(isVReg() && "Wrong type for accessor");
    return RegId;
}

unsigned MachineOperand::getPhysReg() const {
    assert(isPhysReg() && "Wrong type for accessor");
    return RegId;
}

uint64_t MachineOperand::getImm() const {
    assert(isImm() && "Wrong type for accessor");
    return Imm;
}

MachineBB *MachineOperand::getMBB() const {
    assert(isMBB() && "Wrong type for accessor");
    return MBB;
}

MachineInst *MachineOperand::getMI() const {
    return MI;
}

void MachineOperand::setMI(MachineInst *NewMI) {
    MI = NewMI;
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

bool MachineOperand::isDef() const {
    assert((Type == MOType::VirtReg || Type == MOType::PhysReg)
        && "Checking isDef() on a non register operand makes no sense");
    return IsDef;
}

bool MachineOperand::isUse() const {
    assert((Type == MOType::VirtReg || Type == MOType::PhysReg)
        && "Checking isUse() on a non register operand makes no sense");
    return IsUse;
}

void MachineOperand::setIsDef(bool NewIsDef) {
    assert((Type == MOType::VirtReg || Type == MOType::PhysReg)
        && "Setting setIsDef() on a non register operand makes no sense");
    IsDef = NewIsDef;
}

void MachineOperand::setIsUse(bool NewIsUse) {
    assert((Type == MOType::VirtReg || Type == MOType::PhysReg)
        && "Setting setIsUse() on a non register operand makes no sense");
    IsUse = NewIsUse;
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

std::ostream &operator<<(std::ostream &OS, const MachineOperand &MO) {
    MO.print(OS);
    return OS;
}

std::ostream &operator<<(std::ostream &OS, const Register &Reg) {
    Reg.print(OS);
    return OS;
}

} // namespace Balance
