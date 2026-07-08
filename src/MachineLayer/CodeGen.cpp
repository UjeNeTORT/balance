#include "MIROpcodes.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "MachineBB.h"
#include "MachineFunction.h"

#include <iostream>

using namespace Balance;

int main() {

    MachineFunction MFMain("main");

    MachineBB MBB0(&MFMain);
    MachineBB MBB1(&MFMain);

    Register Reg0 = 0;
    Register Reg1 = 1;

    MachineInst AddReg1Reg0 = MachineInst(RISCVOpcode::ADD).addReg(Reg0).addReg(Reg1);
    MachineInst AuipcReg1Reg0 = MachineInst(RISCVOpcode::AUIPC).addReg(Reg0);
    MachineInst Jmp = MachineInst(RISCVOpcode::JAL).addMBB(&MBB0);
    MBB0.InsertMI(&AddReg1Reg0);
    MBB0.InsertMI(&AddReg1Reg0);
    MBB1.InsertMI(&AuipcReg1Reg0);
    MBB1.InsertMI(&AddReg1Reg0);
    MBB1.InsertMI(&Jmp);

    MFMain.InsertMBB(&MBB0);
    MFMain.InsertMBB(&MBB1);

    MFMain.print(std::cout);

  return 0;
}
