#include "MachineInst.h"
#include "MachineOperand.h"
#include "MachineBB.h"
#include "MachineFunction.h"

#include "Utils/Utils.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
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
    return std::holds_alternative<int64_t>(Value);
}

bool MachineOperand::isMBB() const {
    return std::holds_alternative<MachineBB *>(Value);
}

bool MachineOperand::isFunc() const {
    return std::holds_alternative<MachineFunction *>(Value);
}

bool MachineOperand::isLabel() const {
    return std::holds_alternative<std::string>(Value);
}

Register MachineOperand::getReg() const {
    assert(isReg() && "Wrong type for accessor");
    return *std::get_if<Register>(&Value);
}

uint64_t MachineOperand::getImm() const {
    assert(isImm() && "Wrong type for accessor");
    return *std::get_if<int64_t>(&Value);
}

MachineBB *MachineOperand::getMBB() const {
    assert(isMBB() && "Wrong type for accessor");
    return *std::get_if<MachineBB *>(&Value);
}

MachineFunction *MachineOperand::getFunc() const {
    assert(isFunc() && "Wrong type for accessor");
    return *std::get_if<MachineFunction *>(&Value);
}

std::string_view MachineOperand::getLabel() const {
    assert(isLabel() && "Wrong type for accessor");
    return *std::get_if<std::string>(&Value);
}

Register MachineOperand::setReg(Register NewReg) {
    assert(isReg() && "Wrong type for setter");
    return std::exchange(*std::get_if<Register>(&Value), NewReg);
}

uint64_t MachineOperand::setImm(uint64_t NewImm) {
    assert(isImm() && "Wrong type for setter");
    return std::exchange(*std::get_if<int64_t>(&Value), NewImm);
}

MachineBB *MachineOperand::setMBB(MachineBB *NewMBB) {
    assert(isMBB() && "Wrong type for setter");
    return std::exchange(*std::get_if<MachineBB *>(&Value), NewMBB);
}

MachineFunction *MachineOperand::setFunc(MachineFunction *NewFunc) {
    assert(isFunc() && "Wrong type for setter");
    return std::exchange(*std::get_if<MachineFunction *>(&Value), NewFunc);
}

std::string MachineOperand::setLabel(std::string Label) {
    assert(isLabel() && "Wrong type for setter");
    return std::exchange(*std::get_if<std::string>(&Value), Label);
}

MachineInst *MachineOperand::getMI() const {
    return MI;
}

void MachineOperand::setMI(MachineInst *NewMI) {
    MI = NewMI;
}

std::string MachineOperand::getAsmString() const {
    return std::visit(overloaded {
        [](Register Val)         { return Val.getAsmString(); },
        [](int64_t Val)          { return std::to_string(Val); },
        [](MachineBB *Val)       { return std::string(Val->getReferenceName()); },
        [](MachineFunction* Val) { return std::string(Val->getName()); },
        [](std::string Val)      { return Val; }
    }, Value);
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
