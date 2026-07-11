#ifndef MACHINE_INST_H
#define MACHINE_INST_H

#include "MIROpcodes.h"
#include "MachineOperand.h"

#include <cstdint>
#include <vector>

namespace Balance {

class Register;
class MachineBB;

class MachineInst {
    RISCVOpcode Opcode;

    std::vector<MachineOperand> Operands;
    using iterator = std::vector<MachineOperand>::iterator;
    using const_iterator = std::vector<MachineOperand>::const_iterator;

    bool IsDef;
    bool IsUse;
    bool IsTerminator = false; // whether terminates MBB

    MachineBB *MBB = nullptr;

public:
    MachineInst(RISCVOpcode Opcode) : Opcode(Opcode) {}
    MachineInst &addReg(Register Reg);
    MachineInst &addImm(uint64_t Imm);
    MachineInst &addMBB(MachineBB *MBB);

    RISCVOpcode getOpcode() const { return Opcode; }
    MachineBB *getMBB() const { return MBB; }
    void setMBB(MachineBB *NewMBB) { MBB = NewMBB;}

    std::vector<MachineOperand> &getOperands() { return Operands; }
    const std::vector<MachineOperand> &getOperands() const { return Operands; }

    void print(std::ostream &OS) const;

    iterator begin() { return Operands.begin(); }
    iterator end()   { return Operands.end(); }

    const_iterator begin() const { return Operands.begin(); }
    const_iterator end()   const { return Operands.end(); }
};

} // namespace Balance

std::ostream &operator<<(std::ostream &OS, const Balance::MachineInst &MI);

#endif // MACHINE_INST_H
