#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "../RISCV/RISCVRegisters.h"

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <variant>

namespace Balance {

class MachineBB;
class MachineInst;
class MachineOperand {
    enum class MOType {
        VirtReg,
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
    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : MI(MI) {}

    static MachineOperand createVReg(unsigned RegId);
    static MachineOperand createPhysReg(RISCV::RegistersX Reg);
    static MachineOperand createImm(uint64_t Imm);
    static MachineOperand createMBB(MachineBB *MBB);

    std::string_view getAsmString() const;
    void print(std::ostream &OS) const;
};

// general register class
class Register {
    unsigned RegId;
    enum State { Define, Used, LastUsed } State;

public:
    Register(unsigned RegId) : RegId(RegId) {}
    unsigned getId() const { return RegId; }
    void print(std::ostream &OS) const { OS << "VReg" << RegId; }
};

} // namespace Balance

#endif // MACHINE_OPERAND_H
