#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "RISCV/RISCVRegisters.h"

#include <cstdint>
#include <iostream>

namespace Balance {

class MachineBB;
class MachineInst;

enum class MOType {
    VirtReg,
    PhysReg,
    Imm,
    MachineBB
};

class MachineOperand {
    MOType Type;

    union {
        unsigned RegId;
        uint64_t Imm;
        MachineBB *MBB;
    };

    MachineInst *MI;
    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : MI(MI) {}

    static MachineOperand createVReg(unsigned RegId);
    static MachineOperand createPhysReg(RISCV::RegistersX Reg);
    static MachineOperand createImm(uint64_t Imm);
    static MachineOperand createMBB(MachineBB *MBB);

    MachineInst *getMI() const { return MI; }
    void setMI(MachineInst *NewMI) { MI = NewMI; }

    MOType getType() const { return Type; }
    unsigned getRegId() const { return RegId; }
    uint64_t getImm() const { return Imm; }
    MachineBB *getMBB() const { return MBB; }

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
};

bool isVirtReg(const MachineOperand &MO);

bool isDef(const MachineOperand &MO);
bool isUse(const MachineOperand &MO);

void operator<<(std::ostream &OS, const MachineOperand &MO);

} // namespace Balance

#endif // MACHINE_OPERAND_H
