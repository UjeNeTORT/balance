#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "Register.h"

#include <cstdint>
#include <iostream>
#include <variant>

namespace Balance {

class MachineBB;
class MachineInst;

class MachineOperand {
    std::variant<Register, uint64_t, MachineBB *> Value;

    MachineInst *MI;

    bool IsDef = false;
    bool IsUse = false;

    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : Value(Register(0)), MI(MI) {}
    MachineOperand(Register R, MachineInst *MI = nullptr) : Value(R), MI(MI) {}
    MachineOperand(uint64_t Imm, MachineInst *MI = nullptr) : Value(Imm), MI(MI) {}
    MachineOperand(MachineBB *MBB, MachineInst *MI = nullptr) : Value(MBB), MI(MI) {}

    bool isReg() const;
    bool isImm() const;
    bool isMBB() const;

    Register getReg() const;
    uint64_t getImm() const;
    MachineBB *getMBB() const;

    Register setReg(Register NewReg);
    uint64_t setImm(uint64_t NewImm);
    MachineBB *setMBB(MachineBB *NewMBB);

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
