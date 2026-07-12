#include "MIROpcodes.h"
#include "MIRPasses/VerifierPass.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "PassManager.h"

#include "RISCV/RISCVRegisters.h"

#include <iostream>
#include <memory>

using namespace Balance;

MachineFunction createTestMF() {
    MachineFunction MFMain("main");

    MachineBB *MBB0 = MFMain.createMBB("entry");
    MachineBB *MBB1 = MFMain.createMBB("exit");

    Register Reg0 = 0;
    Register Reg1 = 1;
    Register Reg2 = 2;
    Register Reg3 = 3;
    Register Reg4 = 4;
    Register Reg5 = 4;
    Register RA = RISCV::RegistersX::RA;
    Register RZ = RISCV::RegistersX::ZERO;

    MachineInst AddiReg016     = MachineInst(RISCVOpcode::ADDI).addReg(Reg0).addReg(RZ).addImm(0x10);
    MachineInst AuipcReg10x100 = MachineInst(RISCVOpcode::AUIPC).addReg(Reg1).addImm(0x100);
    MachineInst AddReg1Reg0    = MachineInst(RISCVOpcode::ADD).addReg(Reg2).addReg(Reg0).addReg(Reg1);
    MachineInst Jmp            = MachineInst(RISCVOpcode::JAL).addReg(RA).addMBB(MBB0);

    MBB0->insertMI(AddiReg016);
    MBB0->insertMI(AuipcReg10x100);
    // MBB0->createMI(RISCVOpcode::JAL).addMBB(MBB1);

    MBB0->addSuccessor(MBB1);

    MBB1->createMI(RISCVOpcode::AUIPC).addReg(Reg3).addImm(0x200);
    MBB1->createMI(RISCVOpcode::ADD).addReg(Reg5).addReg(Reg2).addReg(Reg3);
    MBB1->insertMI(Jmp);

    MFMain.print(std::cout);
    return MFMain;
}

int main() {
    MachineFunction TestMF = createTestMF();

    PassManager PM;

    PM.registerPass(std::make_unique<VerifierPass>());

    PM.run(TestMF);

    return 0;
}
