#include "MachineOperand.h"
#include "MachineBB.h"

#include "Utils/Utils.h"

#include <cassert>
#include <iostream>
#include <string>
#include <variant>

namespace Balance {

bool isReservedRegister(Register R) {
    if (R.getType() != Register::Type::Physical) return false;
    return isReservedRegister(static_cast<RISCV::RISCVRegister>(R.getId()));
}

bool MachineOperand::isReg() const {
    return std::holds_alternative<Register>(Value);
}

bool MachineOperand::isImm() const {
    return std::holds_alternative<uint64_t>(Value);
}

bool MachineOperand::isMBB() const {
    return std::holds_alternative<MachineBB *>(Value);
}

Register MachineOperand::getReg() const {
    assert(isReg() && "Wrong type for accessor");
    return *std::get_if<Register>(&Value);
}

uint64_t MachineOperand::getImm() const {
    assert(isImm() && "Wrong type for accessor");
    return *std::get_if<uint64_t>(&Value);
}

MachineBB *MachineOperand::getMBB() const {
    assert(isMBB() && "Wrong type for accessor");
    return *std::get_if<MachineBB *>(&Value);
}

Register MachineOperand::setReg(Register NewReg) {
    assert(isReg() && "Wrong type for setter");
    Register OldReg = *std::get_if<Register>(&Value);
    *std::get_if<Register>(&Value) = NewReg;
    return OldReg;
}

uint64_t MachineOperand::setImm(uint64_t NewImm) {
    assert(isImm() && "Wrong type for setter");
    uint64_t OldImm = *std::get_if<uint64_t>(&Value);
    *std::get_if<uint64_t>(&Value) = NewImm;
    return OldImm;
}

MachineBB *MachineOperand::setMBB(MachineBB *NewMBB) {
    assert(isMBB() && "Wrong type for setter");
    MachineBB *OldMBB = *std::get_if<MachineBB *>(&Value);
    *std::get_if<MachineBB *>(&Value) = NewMBB;
    return OldMBB;
}

MachineInst *MachineOperand::getMI() const {
    return MI;
}

void MachineOperand::setMI(MachineInst *NewMI) {
    MI = NewMI;
}

std::string MachineOperand::getAsmString() const {
    std::string AsmString;

    switch (Value.index()) {
    case 0: // Register
        AsmString += std::get<0>(Value).getAsmString();
        break;
    case 1: // Imm
        AsmString += std::to_string(std::get<1>(Value));
        break;
    case 2: // MachineBB *
        AsmString += std::get<2>(Value)->getReferenceName();
        break;
    default:
        unreachable("Unexpected variant index");
        break;
    }

    return AsmString;
}

void MachineOperand::print(std::ostream &OS) const {
    OS << getAsmString();
}

bool MachineOperand::isDef() const {
    assert(isReg() && "Checking isDef() on a non register operand makes no sense");
    return IsDef;
}

bool MachineOperand::isUse() const {
    assert(isReg() && "Checking isUse() on a non register operand makes no sense");
    return IsUse;
}

void MachineOperand::setIsDef(bool NewIsDef) {
    assert(isReg() && "Setting setIsDef() on a non register operand makes no sense");
    IsDef = NewIsDef;
}

void MachineOperand::setIsUse(bool NewIsUse) {
    assert(isReg() && "Setting setIsUse() on a non register operand makes no sense");
    IsUse = NewIsUse;
}

bool MachineOperand::operator==(const MachineOperand &MO2) const {
    if (Value != MO2.Value) return false;
    if (MI != MO2.MI) return false;
    return true;
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
