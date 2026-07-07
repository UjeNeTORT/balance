#ifndef MACHINE_INST_H
#define MACHINE_INST_H

#include "MIROpcodes.h"
#include "MachineOperand.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace Balance {

class Register;
class MachineBB;

class MachineInst {
    RISCVOpcode Opcode;

    std::vector<MachineOperand> Operands;

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
    MachineBB *&getMBB() { return MBB; }

    void print(std::ostream &OS) const;
};

} // namespace Balance

std::ostream &operator<<(std::ostream &OS, const Balance::MachineInst &MI);

#endif // MACHINE_INST_H
