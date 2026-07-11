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
    Register RA = RISCV::RegistersX::RA;

    MachineInst AddReg1Reg0 = MachineInst(RISCVOpcode::ADD).addReg(Reg0).addReg(Reg1).addReg(Reg2);
    MachineInst AuipcReg1Reg0 = MachineInst(RISCVOpcode::AUIPC).addReg(Reg0).addImm(0x100);
    MachineInst Jmp = MachineInst(RISCVOpcode::JAL).addReg(RA).addMBB(MBB0);

    MBB0->createMI(RISCVOpcode::ADD).addReg(Reg0).addReg(Reg1).addReg(Reg2);
    MBB0->insertMI(AddReg1Reg0);
    MBB0->insertMI(AddReg1Reg0);
    // MBB0->createMI(RISCVOpcode::JAL).addMBB(MBB1);

    MBB0->addSuccessor(MBB1);

    MBB1->insertMI(AuipcReg1Reg0);
    MBB1->insertMI(AddReg1Reg0);
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
