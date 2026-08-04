#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "Register.h"

#include <cstdint>
#include <iostream>
#include <variant>

namespace Balance {

class MachineBB;
class MachineInst;
class MachineFunction;

class MachineOperand {
    std::variant<Register, int64_t, MachineBB *, MachineFunction*, std::string> Value;

    MachineInst *MI;

    bool IsDef = false;
    bool IsUse = false;

    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : Value(Register(0)), MI(MI) {}
    MachineOperand(Register R, MachineInst *MI = nullptr) : Value(R), MI(MI) {}
    MachineOperand(int64_t Imm, MachineInst *MI = nullptr) : Value(Imm), MI(MI) {}
    MachineOperand(MachineBB *MBB, MachineInst *MI = nullptr) : Value(MBB), MI(MI) {}
    MachineOperand(MachineFunction *MF, MachineInst *MI = nullptr) : Value(MF), MI(MI) {}
    MachineOperand(std::string Label, MachineInst *MI = nullptr) : Value(Label), MI(MI) {}

    bool isReg() const;
    bool isImm() const;
    bool isMBB() const;
    bool isFunc() const;
    bool isLabel() const;

    Register getReg() const;
    uint64_t getImm() const;
    MachineBB *getMBB() const;
    MachineFunction *getFunc() const;
    std::string_view getLabel() const;

    Register setReg(Register NewReg);
    uint64_t setImm(uint64_t NewImm);
    MachineBB *setMBB(MachineBB *NewMBB);
    MachineFunction *setFunc(MachineFunction *NewFunc);
    std::string setLabel(std::string Label);

    MachineInst *getMI() const;
    void setMI(MachineInst *NewMI);

    bool isDef() const;
    bool isUse() const;

    void setIsDef(bool NewIsDef = true);
    void setIsUse(bool NewIsUse = true);

    std::string getAsmString() const;
    void print(std::ostream &OS) const;

    bool operator==(const MachineOperand &MO2) const;
    bool operator!=(const MachineOperand &MO2) const;
};

std::ostream &operator<<(std::ostream &OS, const MachineOperand &MO);
std::ostream &operator<<(std::ostream &OS, const Register &Reg);

} // namespace Balance
#endif // MACHINE_OPERAND_H
