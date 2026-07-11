#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "RISCV/RISCVRegisters.h"

#include <cstdint>
#include <iostream>

namespace Balance {

class MachineBB;
class MachineInst;

class MachineOperand {
    enum class MOType {
        VirtReg, // TODO: make reg type distinction inside Register struct?
        PhysReg,
        Imm,
        MachineBB
    } Type;
    union {
        unsigned RegId;
        uint64_t Imm;
        MachineBB *MBB;
    };

    MachineInst *MI;

    bool IsDef = false;
    bool IsUse = false;

    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : MI(MI) {}

    static MachineOperand createVReg(unsigned RegId);
    static MachineOperand createPhysReg(RISCV::RegistersX Reg);
    static MachineOperand createImm(uint64_t Imm);
    static MachineOperand createMBB(MachineBB *MBB);

    bool isVReg() const;
    bool isPhysReg() const;
    bool isImm() const;
    bool isMBB() const;

    unsigned getVReg() const;
    unsigned getPhysReg() const;
    uint64_t getImm() const;
    MachineBB *getMBB() const;

    MachineInst *getMI() const;
    void setMI(MachineInst *NewMI);

    bool isDef() const;
    bool isUse() const;

    void setIsDef(bool NewIsDef = true);
    void setIsUse(bool NewIsUse = true);

    std::string_view getAsmString() const;
    void print(std::ostream &OS) const;

    bool operator==(const MachineOperand &MO2) const;
    bool operator!=(const MachineOperand &MO2) const;
};

// general register class
class Register {
    unsigned RegId;
    enum State { Define, Used, LastUsed } State;

public:
    Register(unsigned RegId) : RegId(RegId) {}
    unsigned getId() const { return RegId; }
    void print(std::ostream &OS) const { OS << "%VReg" << RegId; }

    bool operator==(const Register &Reg2) const { return RegId == Reg2.RegId; }
    bool operator!=(const Register &Reg2) const { return !(*this == Reg2); }
};

std::ostream &operator<<(std::ostream &OS, const MachineOperand &MO);
std::ostream &operator<<(std::ostream &OS, const Register &Reg);

} // namespace Balance

#endif // MACHINE_OPERAND_H
