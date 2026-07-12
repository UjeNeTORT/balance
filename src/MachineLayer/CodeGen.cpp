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

// ai-generated
MachineFunction createDiamondTestMF() {
    MachineFunction MFMain("main_diamond");

    // Create BBs for a diamond shape CFG
    MachineBB *MBB0 = MFMain.createMBB("entry");
    MachineBB *MBB1 = MFMain.createMBB("if.true");
    MachineBB *MBB2 = MFMain.createMBB("if.false");
    MachineBB *MBB3 = MFMain.createMBB("if.end");

    // Setup CFG:
    // MBB0 -> MBB1 (fallthrough), MBB2 (taken)
    // MBB1 -> MBB3
    // MBB2 -> MBB3
    MBB0->addSuccessor(MBB1);
    MBB0->addSuccessor(MBB2);
    MBB1->addSuccessor(MBB3);
    MBB2->addSuccessor(MBB3);

    // Define some registers
    Register Reg0 = 0;
    Register Reg1 = 1;
    Register Reg2 = 2;
    Register Reg4 = 4;
    Register Reg5 = 5;
    Register RZ = RISCV::RISCVRegister::ZERO;

    // MBB0: entry block with a conditional branch
    // if (Reg0 != 0) goto if.false;
    MBB0->createMI(RISCVOpcode::ADDI).addReg(Reg0).addReg(RZ).addImm(0);
    MBB0->createMI(RISCVOpcode::BNE).addReg(Reg0).addReg(RZ).addMBB(MBB2);

    // MBB1: if.true block
    // fallthrough from MBB0
    MBB1->createMI(RISCVOpcode::ADDI).addReg(Reg1).addReg(RZ).addImm(10);
    MBB1->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB3);

    // MBB2: if.false block
    MBB2->createMI(RISCVOpcode::ADDI).addReg(Reg2).addReg(RZ).addImm(20);
    MBB2->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB3);

    // MBB3: if.end block with a PHI node
    // Reg4 = PHI [Reg1, MBB1], [Reg2, MBB2]
    MBB3->createMI(RISCVOpcode::PHI).addReg(Reg4).addReg(Reg1).addMBB(MBB1).addReg(Reg2).addMBB(MBB2);

    // Use the result of the PHI
    MBB3->createMI(RISCVOpcode::ADD).addReg(Reg5).addReg(Reg4).addReg(RZ);

    MFMain.print(std::cout);

    return MFMain;
}

MachineFunction createTestMF() {
    MachineFunction MFMain("main");

    MachineBB *MBB0 = MFMain.createMBB("entry");
    MachineBB *MBB1 = MFMain.createMBB("exit");

    MBB0->addSuccessor(MBB1);

    Register Reg0 = 0;
    Register Reg1 = 1;
    Register Reg2 = 2;
    Register RA = RISCV::RISCVRegister::RA;
    Register RZ = RISCV::RISCVRegister::ZERO;

    MBB0->createMI(RISCVOpcode::ADDI).addReg(Reg0).addReg(RZ).addImm(0x10);
    MBB0->createMI(RISCVOpcode::AUIPC).addReg(Reg1).addImm(0x100);
    MBB0->createMI(RISCVOpcode::JAL).addReg(RA).addMBB(MBB1);

    MBB1->createMI(RISCVOpcode::ADD).addReg(Reg2).addReg(Reg0).addReg(Reg1);

    MFMain.print(std::cout);
    return MFMain;
}

int main() {
    MachineFunction TestMF = createDiamondTestMF();

    PassManager PM;

    PM.registerPass(std::make_unique<VerifierPass>());

    PM.run(TestMF);

    return 0;
}
