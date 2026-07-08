#include "MachineInst.h"
#include "MachineOperand.h"
#include "MIROpcodes.h"

#include <algorithm>
#include <ostream>

using namespace Balance;

class Register;

MachineInst &MachineInst::addReg(Register Reg) {
  Operands.push_back(MachineOperand::createVReg(Reg.getId()));
  return *this;
}

MachineInst &MachineInst::addImm(uint64_t Imm) {
  Operands.push_back(MachineOperand::createImm(Imm));
  return *this;
}

MachineInst &MachineInst::addMBB(MachineBB *MBB) {
  Operands.push_back(MachineOperand::createMBB(MBB));
  return *this;
}

void MachineInst::print(std::ostream &OS) const {
  OS << getInstNameByOpcode(Opcode) << " ";
  bool First = true;

  std::for_each(Operands.begin(), Operands.end(),
    [&First, &OS](MachineOperand Op) {
      if ( First ) First = false; else OS << ", ";
      Op.print(OS);
    }
  );
}

std::ostream &operator<<(std::ostream &OS, const MachineInst &MI) {
  MI.print(OS);
  return OS;
}
