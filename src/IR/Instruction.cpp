#include "Instruction.h"

#include <variant>

using namespace Balance;
using namespace Opcodes;

void Instruction::verify() const {
    if (Opcode != NOP)
        throw verify_error("Basic Instruction must be NOP");
}

void UnaryInstruction::verify() const {
    if (Opcode == CONVERT) {
        if ((std::holds_alternative<IntVirtRegister>(Dst) &&
             !std::holds_alternative<float>(Src) && !std::holds_alternative<FloatVirtRegister>(Src)) ||
            (!std::holds_alternative<int>(Src) && !std::holds_alternative<IntVirtRegister>(Src)))
                throw verify_error("CONVERT instruction must have different type Dst and Src");

    } else if (Opcode == BITCAST) {
        if (std::holds_alternative<int>(Src) || std::holds_alternative<float>(Src))
            throw verify_error("BITCAST instruction mustn't hold constant value");

        if (Src.index() == Dst.index())
            throw verify_error("BITCAST instruction must have different type Dst and Src");
    } else if (Opcode == COPY) {
        if ((std::holds_alternative<IntVirtRegister>(Dst) &&
             !std::holds_alternative<int>(Src) && !std::holds_alternative<IntVirtRegister>(Src)) ||
            (!std::holds_alternative<float>(Src) && !std::holds_alternative<FloatVirtRegister>(Src)))
                throw verify_error("COPY instruction must have same type Dst and Src");

    } else if (Opcode == NEG) {
        if (Src.index() != Dst.index())
            throw verify_error("NEG instruction Src must be same type with Dst");
    } else {
        throw verify_error("Invalid Opcode for UnaryInstruction");
    }
}

void BinaryInstruction::verify() const {
    if (Opcode == ADD || Opcode == SUB || Opcode == MUL || Opcode == DIV || Opcode == REM ||
        Opcode == SHL || Opcode == SHR || Opcode == AND || Opcode == OR || Opcode == XOR) {

        if (CmpType.has_value())
            throw verify_error("CmpType is given for non CMP instruction");
    } else if (Opcode == CMP) {

        if (!CmpType.has_value())
            throw verify_error("CmpType is not given for CMP instruction");
    } else {
        throw verify_error("Invalid Opcode for BinaryInstruction");
    }

    if (Src.index() != Dst.index()) {
        throw verify_error("BinaryInstruction Dst and Src must have same type");
    }
}

void RetInstruction::verify() const {
    if (Opcode != RET)
        throw verify_error("RetInstruction must have RET Opcode");
}

void BrInstruction::verify() const {
    if (Opcode != BR)
        throw verify_error("BrInstruction must have BR Opcode");

    if (DstBB == nullptr)
        throw verify_error("BrInstruction must have destination basic block");
}

void LoadInstruction::verify() const {
    if (Opcode != LOAD)
        throw verify_error("LoadInstruction must have LOAD Opcode");
}

void StoreInstruction::verify() const {
    if (Opcode != STORE)
        throw verify_error("StoreInstruction must have STORE Opcode");
}

void CallInstruction::verify() const {
    if (Opcode != CALL)
        throw verify_error("CallInstruction must have CALL Opcode");

    if (FunctionBB == nullptr)
        throw verify_error("CallInstruction must point to function basic block");
}

void PhiInstruction::verify() const {
    if (Opcode != PHI)
        throw verify_error("PhiInstruction must have PHI Opcode");

    if (Arguments.size() < 2)
        throw verify_error("PhiInstruction must have at least 2 Arguments");

    for (auto arg: Arguments) {
        if (arg.index() != Dst.index())
            throw verify_error("Arguments in PhiInstruction must have same type with Dst");
    }
}

