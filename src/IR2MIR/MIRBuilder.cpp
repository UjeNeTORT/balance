#include "MIRBuilder.h"
#include "MachineLayer/MachineInst.h"
#include "RISCV/RISCVRegisters.h"
#include "UniversalAnalysis/RPOTraversal.h"

#include <cstdint>
#include <cstring>
#include <map>
#include <variant>

using namespace Balance;
using namespace RISCV;
using RVReg = RISCVRegister;
using RVOp = RISCVOpcode;

MIR MIRBuilder::build() && {
    for (auto& Func: IntermRepr)
        FuncRegistry[&Func] = MachineIR.addFunction(MachineFunction(std::string(Func.getName())));

    auto IRIt = IntermRepr.begin();
    auto MIRIt = MachineIR.begin();
    for (; IRIt != IntermRepr.end() && MIRIt != MachineIR.end(); IRIt++, MIRIt++)
        buildFunction(IRIt, MIRIt);

    return MachineIR;
}

void MIRBuilder::buildFunction(const IR::iterator IRIt, const MIR::iterator MIRIt) {
    auto RPO = RPOTraversal<BasicBlock, Function>(*IRIt).getRPO();

    std::map<const BasicBlock*, MachineBB*> BBRegistry;

    for (auto& Block: RPO)
        BBRegistry[Block] = MIRIt->createMBB(std::string(Block->getName()));

    auto IRBlock = RPO.begin();
    auto MIRBlock = MIRIt->begin();

    ssize_t AllocasSize = -1;

    for (; IRBlock != RPO.end() && MIRBlock != MIRIt->end(); IRBlock++, MIRBlock++)
        buildBasicBlock(*IRBlock, &*MIRBlock, BBRegistry, &AllocasSize);
}

namespace {

void createBinaryOp(MachineBB* MIRBlock, RISCVOpcode IntOpcode, RISCVOpcode FloatOpcode,
                    const std::vector<VirtRegister>& Dst, const std::vector<VirtRegister>& Src) {
    if (Dst[0].Type == VirtRegister::Int)
        MIRBlock->createMI(IntOpcode).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
    else
        MIRBlock->createMI(FloatOpcode).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
}

size_t createFuncDef(MachineBB* MIRBlock, int FrameSize, const std::vector<VirtRegister>& Dst) {
    size_t IRegCnt = 0;
    size_t FRegCnt = 0;
    size_t StackCnt = 0;

    for (auto It = std::next(Dst.begin()); It != Dst.end(); It++) {
        if (It->Type == VirtRegister::Int) {
            if (IRegCnt < CallIntArgsRegs.size())
                MIRBlock->createMI(RVOp::ADDI).addReg(*It).addReg(CallIntArgsRegs[IRegCnt]).addImm(0);
            else
                MIRBlock->createMI(RVOp::LW).addReg(*It).addReg(RVReg::SP).addImm(8 * StackCnt++); // TODO: check overflow

            IRegCnt++;
        } else {
            if (FRegCnt < CallFloatArgsRegs.size())
                MIRBlock->createMI(RVOp::FSGNJ_S).addReg(*It).addReg(CallFloatArgsRegs[FRegCnt])
                                                             .addReg(CallFloatArgsRegs[FRegCnt]);
            else
                MIRBlock->createMI(RVOp::FLW).addReg(*It).addReg(RVReg::SP).addImm(8 * StackCnt++); // TODO: check overflow

            FRegCnt++;
        }
    }

    MIRBlock->createMI(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP)
                                  .addImm(-static_cast<int64_t>(FrameSize)); // TODO: check overflow
    MIRBlock->createMI(RVOp::ADDI).addReg(Dst[0]).addReg(RVReg::SP).addImm(0);

    return FrameSize;
}

void createCall(MachineBB* MIRBlock, MachineFunction* MFunc, const std::vector<VirtRegister>& Src, const std::vector<VirtRegister>& Dst) {
    size_t StackArgsCnt = 0;
    size_t IRegCnt = 0;
    size_t FRegCnt = 0;

    for (const auto& Arg: Src) {
        if (Arg.Type == VirtRegister::Int) {
            if (++IRegCnt > CallIntArgsRegs.size())
                StackArgsCnt++;
        } else {
            if (++FRegCnt > CallFloatArgsRegs.size())
                StackArgsCnt++;
        }
    }

    size_t StackShift = (8 + (StackArgsCnt * 8) + 15) & ~15;

    MIRBlock->createMI(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP).addImm(-StackShift); // TODO: check overflow
    MIRBlock->createMI(RVOp::SD).addReg(RVReg::RA).addReg(RVReg::SP).addImm(StackShift - 8);

    StackArgsCnt = 0;
    IRegCnt = 0;
    FRegCnt = 0;
    for (const auto& Arg: Src) {
        if (Arg.Type == VirtRegister::Int) {
            if (IRegCnt < CallIntArgsRegs.size())
                MIRBlock->createMI(ADDI).addReg(CallIntArgsRegs[IRegCnt]).addReg(Arg).addImm(0);
            else
                MIRBlock->createMI(RVOp::SW).addReg(RVReg::SP).addImm(8 * StackArgsCnt++).addReg(Arg);

            IRegCnt++;
        } else {
            if (FRegCnt < CallFloatArgsRegs.size())
                MIRBlock->createMI(RVOp::FSGNJ_S).addReg(CallFloatArgsRegs[FRegCnt]).addReg(Arg).addReg(Arg);
            else
                MIRBlock->createMI(RVOp::FSW).addReg(RVReg::SP).addImm(8 * StackArgsCnt++).addReg(Arg);

            FRegCnt++;
        }
    }

    MIRBlock->createMI(RVOp::CALL).addReg(RVReg::RA).addFunc(MFunc);

    if (Dst.size() == 1)
        MIRBlock->createMI(RVOp::ADDIW).addReg(Dst[0]).addReg(RVReg::A0).addImm(0);

    MIRBlock->createMI(RVOp::LD).addReg(RVReg::RA).addReg(RVReg::SP).addImm(StackShift - 8);
    MIRBlock->createMI(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP).addImm(StackShift); // TODO: check overflow
}

} // anonymous namespace

void MIRBuilder::buildBasicBlock(BasicBlock* IRBlock, MachineBB* MIRBlock,
                                 std::map<const BasicBlock*, MachineBB*>& BBRegistry,
                                 ssize_t* AllocasSize) {
    for (const auto& Instr: *IRBlock) {
        auto Src = Instr.getSrc();
        auto Dst = Instr.getDst();
        auto Imm = Instr.getImm();
        auto CmpType = Instr.getCmpType();
        auto BrDstBB = Instr.getBrDstBB();
        auto CallFunc = Instr.getCallFunc();

        switch (Instr.getOpcode()) {
            case Opcodes::NOP:
                MIRBlock->createMI(RVOp::ADDI).addReg(RVReg::ZERO).addReg(RVReg::ZERO).addImm(0);

            break; case Opcodes::CONVERT:
                if (Dst[0].Type == VirtRegister::Int)
                    MIRBlock->createMI(RVOp::FCVT_W_S).addReg(Dst[0]).addReg(Src[0]);
                else
                    MIRBlock->createMI(RVOp::FCVT_S_W).addReg(Dst[0]).addReg(Src[0]);

            break; case Opcodes::BITCAST:
                if (Dst[0].Type == VirtRegister::Int)
                    MIRBlock->createMI(RVOp::FMV_W_X).addReg(Dst[0]).addReg(Src[0]);
                else
                    MIRBlock->createMI(RVOp::FMV_X_W).addReg(Dst[0]).addReg(Src[0]);

            break; case Opcodes::COPY:
                if (Src.size() == 1) {
                    if (Dst[0].Type == VirtRegister::Int)
                        MIRBlock->createMI(RVOp::ADDIW).addReg(Dst[0]).addReg(Src[0]).addImm(0);
                    else
                        MIRBlock->createMI(RVOp::FSGNJ_S).addReg(Dst[0]).addReg(Src[0]).addReg(Src[0]);
                } else {
                    if (Dst[0].Type == VirtRegister::Int) {
                        if (std::get_if<int>(&*Imm))
                            MIRBlock->createMI(RVOp::LI).addReg(Dst[0])
                                            .addImm(static_cast<int64_t>(*std::get_if<int>(&*Imm)));
                        else
                            MIRBlock->createMI(RVOp::LI).addReg(Dst[0])
                                            .addLabel(*std::get_if<std::string>(&*Imm));
                    } else {
                        uint32_t Val = 0;
                        std::memcpy(&Val, std::get_if<float>(&*Imm), sizeof(float));
                        VirtRegister Vreg = IRBlock->getParentFunction()->getNewVirtReg(VirtRegister::Int);
                        MIRBlock->createMI(RVOp::LI).addReg(Vreg).addImm(Val);
                        MIRBlock->createMI(RVOp::FMV_X_W).addReg(Dst[0]).addReg(RVReg::RA);
                    }
                }
            break; case Opcodes::NEG:
                if (Dst[0].Type == VirtRegister::Int)
                    MIRBlock->createMI(RVOp::SUBW).addReg(Dst[0]).addReg(RVReg::ZERO).addReg(Src[0]);
                else
                    MIRBlock->createMI(RVOp::FSGNJN_S).addReg(Dst[0]).addReg(Src[0]).addReg(Src[0]);

            break; case Opcodes::ADD:
                createBinaryOp(MIRBlock, RVOp::ADDW, RVOp::FADD_S, Dst, Src);
            break; case Opcodes::SUB:
                createBinaryOp(MIRBlock, RVOp::SUBW, RVOp::FSUB_S, Dst, Src);
            break; case Opcodes::MUL:
                createBinaryOp(MIRBlock, RVOp::MULW, RVOp::FMUL_S, Dst, Src);
            break; case Opcodes::DIV:
                createBinaryOp(MIRBlock, RVOp::DIVW, RVOp::FDIV_S, Dst, Src);
            break; case Opcodes::REM:
                MIRBlock->createMI(RVOp::REMW).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::SHL:
                MIRBlock->createMI(RVOp::SLLW).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::SHR:
                MIRBlock->createMI(RVOp::SRAW).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::AND:
                MIRBlock->createMI(RVOp::AND).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::OR:
                MIRBlock->createMI(RVOp::OR).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::XOR:
                MIRBlock->createMI(RVOp::XOR).addReg(Dst[0]).addReg(Src[0]).addReg(Src[1]);
            break; case Opcodes::RET:
                if (Src.size() == 1)
                    MIRBlock->createMI(RVOp::ADDW).addReg(RVReg::A0).addReg(Src[0]).addReg(RVReg::ZERO);

                assert(*AllocasSize != -1);
                MIRBlock->createMI(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP)
                                              .addImm(static_cast<int64_t>(*AllocasSize)); // TODO: check overflow
                MIRBlock->createMI(RVOp::JALR).addReg(RVReg::ZERO).addReg(RVReg::RA).addImm(0);

            break; case Opcodes::BR:
                if (Src.size() != 0) {
                    RVOp BROpcode;
                    bool RevOp;
                    switch (*CmpType) {
                        case CmpTypes::EQ: BROpcode = RVOp::BEQ; RevOp = false; break;
                        case CmpTypes::NE: BROpcode = RVOp::BNE; RevOp = false; break;
                        case CmpTypes::GE: BROpcode = RVOp::BGE; RevOp = false; break;
                        case CmpTypes::GT: BROpcode = RVOp::BLT; RevOp = true;  break;
                        case CmpTypes::LE: BROpcode = RVOp::BGE; RevOp = true;  break;
                        case CmpTypes::LT: BROpcode = RVOp::BLT; RevOp = false; break;
                        default: assert(0);
                    }
                    if (RevOp)
                        MIRBlock->createMI(BROpcode).addReg(Src[1]).addReg(Src[0]).addMBB(BBRegistry[BrDstBB[1]]);
                    else
                        MIRBlock->createMI(BROpcode).addReg(Src[0]).addReg(Src[1]).addMBB(BBRegistry[BrDstBB[1]]);
                }
                MIRBlock->createMI(RVOp::JAL).addReg(RVReg::ZERO).addMBB(BBRegistry[BrDstBB[0]]);

            break; case Opcodes::LOAD:
                MIRBlock->createMI(RVOp::LW).addReg(Dst[0]).addReg(Src[0]).addImm(0);
            break; case Opcodes::STORE:
                MIRBlock->createMI(RVOp::SW).addReg(Src[0]).addImm(0).addReg(Src[1]);
            break; case Opcodes::CALL:
                createCall(MIRBlock, FuncRegistry[*CallFunc], Src, Dst);
            break; case Opcodes::PHI:
                MIRBlock->createMI(RVOp::PHI).addReg(Dst[0])
                                             .addReg(Src[0]).addMBB(BBRegistry[*Src[0].DefBlock])
                                             .addReg(Src[1]).addMBB(BBRegistry[*Src[1].DefBlock]);
            break; case Opcodes::FUNC_DEF:
                assert(*AllocasSize == -1);
                *AllocasSize = createFuncDef(MIRBlock, *std::get_if<int>(&*Imm), Dst);
            break; default:
                assert(0);
        }
    }
}

