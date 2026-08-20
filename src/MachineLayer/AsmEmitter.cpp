#include "AsmEmitter.h"

#include "MIROpcodes.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineOperand.h"
#include "UniversalAnalysis/RPOTraversal.h"
#include "Utils.h"

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <variant>

using namespace Balance;

void AsmEmitter::emitGlobalVar(const MachineGData& Var) {
    OS << Var.getName() << ":    ";

    std::visit([this](const auto& InitVec){
        using T = typename std::decay_t<decltype(InitVec)>::value_type;
        if constexpr (std::is_same_v<T, uint8_t>)
            OS << ".byte";
        else if constexpr (std::is_same_v<T, uint16_t>)
            OS << ".half";
        else if constexpr (std::is_same_v<T, uint32_t>)
            OS << ".word";
        else if constexpr (std::is_same_v<T, uint64_t>)
            OS << ".dword";
        else
            static_assert(0, "unhandled type");

        OS << "    ";
        for (auto Val = InitVec.begin(); Val != InitVec.end(); Val++) {
            OS << std::hex << std::showbase << *Val << std::dec;

            if (std::next(Val) != InitVec.end())
                OS << ", ";
        }
        OS << "\n";
    }, Var.getInit());
}

void AsmEmitter::emitBBLabel(const MachineBB& BB) {
    OS << ".L_" << BB.getReferenceName() << "_" << BB.getMF()->getName();
}

void AsmEmitter::emitInstrOp(const MachineOperand& Op) {
    std::visit(overloaded {
        [this](Register Val)         { OS << Val.getAsmString(); },
        [this](int64_t Val)          { OS << Val; },
        [this](MachineBB *Val)       { emitBBLabel(*Val); },
        [this](MachineFunction* Val) { OS << Val->getName(); },
        [this](MachineGData* Val)    { OS << Val->getName(); }
    }, Op.getValue());
}

void AsmEmitter::emitInstr(const MachineInst& Instr) {
    OS << "    " << getInstNameByOpcode(Instr.getOpcode());

    using RVOp = RISCVOpcode;
    switch (Instr.getOpcode()) {
        case RVOp::CALL:
            OS << " ";
            emitInstrOp(Instr.getOperands()[1]);
            break;
        case RVOp::RET:
            break;
        case RVOp::LB:
        case RVOp::LBU:
        case RVOp::LH:
        case RVOp::LHU:
        case RVOp::LW:
        case RVOp::LD:
            OS << " ";
            emitInstrOp(Instr.getOperands()[0]);
            OS << ", ";
            emitInstrOp(Instr.getOperands()[2]);
            OS << "(";
            emitInstrOp(Instr.getOperands()[1]);
            OS << ")";
        break;
        case RVOp::SB:
        case RVOp::SH:
        case RVOp::SW:
        case RVOp::SD:
            OS << " ";
            emitInstrOp(Instr.getOperands()[2]);
            OS << ", ";
            emitInstrOp(Instr.getOperands()[1]);
            OS << "(";
            emitInstrOp(Instr.getOperands()[0]);
            OS << ")";
            break;

        default: {
            OS << " ";
            bool First = true;
            std::for_each(Instr.getOperands().begin(), Instr.getOperands().end(),
                [&First, this](const MachineOperand &Op) {
                    if ( First ) First = false;
                    else OS << ", ";

                    emitInstrOp(Op);
                }
            );
         }
    }
    OS << "\n";
}

void AsmEmitter::emitBasicBlock(const MachineBB& BB) {
    emitBBLabel(BB); OS << ":\n";

    for (const auto& Instr: BB)
        emitInstr(Instr);
}

void AsmEmitter::emitFunction(const MachineFunction& Func) {
    if (Func.isDecl())
        return;

    if (Func.getName() == "main")
        OS << ".type main, @function\n";

    OS << Func.getName() << ":\n";

    auto RPO = RPOTraversal<const MachineBB, const MachineFunction>(Func).getRPO();

    for (const auto* BB: RPO)
        emitBasicBlock(*BB);

    OS << "# End of " << Func.getName() << "\n\n";

    if (Func.getName() == "main")
        OS << ".size main, .-main\n\n";
}

void AsmEmitter::emit() && {
    OS << ".section .data\n" <<
          ".align 3\n\n"; //< 64 bit alignment

    for (auto Var = Mir.gdata_begin(); Var != Mir.gdata_end(); Var++)
        if (!Var->isConst())
            emitGlobalVar(*Var);

    OS << "\n" <<
          ".section .rodata\n" <<
          ".align 3\n\n";

    for (auto Var = Mir.gdata_begin(); Var != Mir.gdata_end(); Var++)
        if (Var->isConst())
            emitGlobalVar(*Var);

    OS << "\n" <<
          ".section .text\n" <<
          ".align 2\n" <<
          ".global main\n\n";

    for (const auto& Func: Mir)
        emitFunction(Func);
}

