#include "MIROpcodes.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "PassManager.h"

#include "MIRPasses/VerifierPass.h"
#include "MIRPasses/LivenessAnalysis.h"
#include "MIRPasses/PhiElimination.h"
#include "MIRPasses/LinearScanRAL.h"

#include "RISCV/RISCVRegisters.h"

#include <iostream>
#include <memory>

using namespace Balance;

// ai-generated
MachineFunction createPhiSwapTestMF() {
    /*
    C code that could generate this:
    int main() {
        int a = 10;
        int b = 20;
        for (int i = 0; i < 10; i++) {
            int temp = a;
            a = b;
            b = temp;
        }
        return a;
    }
    */
    MachineFunction MF("phi_swap_test");

    MachineBB *MBB0_preheader = MF.createMBB("preheader");
    MachineBB *MBB1_loop_header = MF.createMBB("loop_header");
    MachineBB *MBB2_loop_body = MF.createMBB("loop_body");
    MachineBB *MBB3_loop_exit = MF.createMBB("loop_exit");

    // CFG:
    // preheader -> loop_header
    // loop_header -> loop_body, loop_exit
    // loop_body -> loop_header
    MBB0_preheader->addSuccessor(MBB1_loop_header);
    MBB1_loop_header->addSuccessor(MBB2_loop_body);
    MBB1_loop_header->addSuccessor(MBB3_loop_exit);
    MBB2_loop_body->addSuccessor(MBB1_loop_header);

    Register RZ = RISCV::RISCVRegister::ZERO;
    Register a0 = MF.getNewVreg();
    Register b0 = MF.getNewVreg();
    Register i0 = MF.getNewVreg();

    // preheader
    MBB0_preheader->createMI(RISCVOpcode::ADDI).addReg(a0).addReg(RZ).addImm(10);
    MBB0_preheader->createMI(RISCVOpcode::ADDI).addReg(b0).addReg(RZ).addImm(20);
    MBB0_preheader->createMI(RISCVOpcode::ADDI).addReg(i0).addReg(RZ).addImm(0);
    MBB0_preheader->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB1_loop_header);

    Register i1 = MF.getNewVreg();
    Register a1 = MF.getNewVreg();
    Register b1 = MF.getNewVreg();

    Register i2 = MF.getNewVreg();
    Register a2 = MF.getNewVreg();
    Register b2 = MF.getNewVreg();

    // loop_header
    // i1 = PHI [i0, preheader], [i2, loop_body]
    // a1 = PHI [a0, preheader], [b2, loop_body]
    // b1 = PHI [b0, preheader], [a2, loop_body]
    MBB1_loop_header->createMI(RISCVOpcode::PHI).addReg(i1).addReg(i0).addMBB(MBB0_preheader).addReg(i2).addMBB(MBB2_loop_body);
    MBB1_loop_header->createMI(RISCVOpcode::PHI).addReg(a1).addReg(a0).addMBB(MBB0_preheader).addReg(b2).addMBB(MBB2_loop_body);
    MBB1_loop_header->createMI(RISCVOpcode::PHI).addReg(b1).addReg(b0).addMBB(MBB0_preheader).addReg(a2).addMBB(MBB2_loop_body);

    Register limit = MF.getNewVreg();
    Register cond = MF.getNewVreg();
    MBB1_loop_header->createMI(RISCVOpcode::ADDI).addReg(limit).addReg(RZ).addImm(10);
    MBB1_loop_header->createMI(RISCVOpcode::SLT).addReg(cond).addReg(i1).addReg(limit);
    MBB1_loop_header->createMI(RISCVOpcode::BEQ).addReg(cond).addReg(RZ).addMBB(MBB3_loop_exit);
    MBB1_loop_header->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB2_loop_body);

    // loop_body
    // a2 = b1; (move)
    MBB2_loop_body->createMI(RISCVOpcode::ADD).addReg(a2).addReg(b1).addReg(RZ);
    // b2 = a1; (move)
    MBB2_loop_body->createMI(RISCVOpcode::ADD).addReg(b2).addReg(a1).addReg(RZ);
    // i2 = i1 + 1
    MBB2_loop_body->createMI(RISCVOpcode::ADDI).addReg(i2).addReg(i1).addImm(1);
    MBB2_loop_body->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB1_loop_header);

    // loop_exit
    // just use one of the results
    Register res = MF.getNewVreg();
    MBB3_loop_exit->createMI(RISCVOpcode::ADD).addReg(res).addReg(a1).addReg(RZ);

    return MF;
}

MachineFunction createDiamondTestMF() {
    MachineFunction MFMain("main_diamond");

    MachineBB *MBB0 = MFMain.createMBB("entry");
    MachineBB *MBB1 = MFMain.createMBB("if.true");
    MachineBB *MBB2 = MFMain.createMBB("if.false");
    MachineBB *MBB3 = MFMain.createMBB("if.end");

    // CFG:
    // MBB0 -> MBB1, MBB2
    // MBB1 -> MBB3
    // MBB2 -> MBB3
    MBB0->addSuccessor(MBB1);
    MBB0->addSuccessor(MBB2);
    MBB1->addSuccessor(MBB3);
    MBB2->addSuccessor(MBB3);

    Register Reg0 = MFMain.getNewVreg();
    Register Reg1 = MFMain.getNewVreg();
    Register Reg2 = MFMain.getNewVreg();
    Register Reg4 = MFMain.getNewVreg();
    Register Reg5 = MFMain.getNewVreg();
    Register RZ = RISCV::RISCVRegister::ZERO;

    // MBB0: entry block with a conditional branch
    // if (Reg0 != 0) goto if.false;
    MBB0->createMI(RISCVOpcode::ADDI).addReg(Reg0).addReg(RZ).addImm(0);
    MBB0->createMI(RISCVOpcode::BNE).addReg(Reg0).addReg(RZ).addMBB(MBB2);
    MBB0->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB1);

    // MBB1: if.true block
    // from MBB0
    MBB1->createMI(RISCVOpcode::ADDI).addReg(Reg1).addReg(RZ).addImm(10);
    MBB1->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB3);

    // MBB2: if.false block
    // fallthrough from MBB0
    MBB2->createMI(RISCVOpcode::ADDI).addReg(Reg2).addReg(RZ).addImm(20);
    MBB2->createMI(RISCVOpcode::JAL).addReg(RZ).addMBB(MBB3);

    // MBB3: if.end block with a PHI node
    // Reg4 = PHI [Reg1, MBB1], [Reg2, MBB2]
    MBB3->createMI(RISCVOpcode::PHI).addReg(Reg4).addReg(Reg1).addMBB(MBB1).addReg(Reg2).addMBB(MBB2);
    MBB3->createMI(RISCVOpcode::ADD).addReg(Reg5).addReg(Reg4).addReg(RZ);

    return MFMain;
}

MachineFunction createTestMF() {
    MachineFunction MFMain("main");

    MachineBB *MBB0 = MFMain.createMBB("entry");
    MachineBB *MBB1 = MFMain.createMBB("exit");

    MBB0->addSuccessor(MBB1);

    Register Reg0 = MFMain.getNewVreg();
    Register Reg1 = MFMain.getNewVreg();
    Register Reg2 = MFMain.getNewVreg();
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
    MachineFunction TestMF = createPhiSwapTestMF();

    PassManager PM;

    PM.registerPass<VerifierPass>();
    PM.registerPass<LivenessAnalysis>();
    PM.registerPass<VerifierPass>();
    // PM.registerPass<PhiElimination>();
    // PM.registerPass<VerifierPass>();

    PassManager PMPhi;
    PMPhi.registerPass<VerifierPass>();
    PMPhi.registerPass<PhiElimination>();
    PMPhi.registerPass<VerifierPass>();
    PMPhi.registerPass<LivenessAnalysis>();
    PMPhi.registerPass<VerifierPass>();
    PMPhi.registerPass<LinearScanRAL>();

    TestMF.print(std::cout);

    PMPhi.run(TestMF);

    TestMF.print(std::cout);

    return 0;
}
