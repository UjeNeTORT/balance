#include "IR/Instruction.h"

#include "IR/BasicBlock.h"
#include "IR/Function.h"
#include "Utils/Utils.h"

#include <cassert>
#include <variant>

using namespace Balance;

void Instruction::throwVerifyError(std::string error) const {
    // TODO: Print instruction info
    throw verify_error(error);
}

void Instruction::verify() const {
    if (ParentBB == nullptr)
        throwVerifyError("Invalid ParentBB");

    switch (Opcode) {
        case Opcodes::NOP:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoSrc();
            verifyNoDst();
            verifyNoBrDstBB();
            verifyNoFunc();
            break;
        case Opcodes::CONVERT:
        case Opcodes::BITCAST:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 1 || Dst.size() != 1)
                throwVerifyError("Unary operation must have 1 source and 1 destination");

            if (Src[0].Type == Dst[0].Type)
                throwVerifyError("CONVERT/BITCAST operation must have different source and destination types");
            break;
        case Opcodes::COPY:
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Immediate.has_value()) {
                if (Src.size() != 0 || Dst.size() != 1)
                    throwVerifyError("COPY operation with immediate must have no source and 1 destination");

                if ((std::holds_alternative<int>(*Immediate) && !std::holds_alternative<OpInt>(Dst[0].Type)) ||
                    (std::holds_alternative<float>(*Immediate) && !std::holds_alternative<OpFloat>(Dst[0].Type)) ||
                    (std::holds_alternative<GlobalData*>(*Immediate) && !Dst[0].Type.isInt()))
                    throwVerifyError("COPY operation must have same source and destination types");
            } else {
                if (Src.size() != 1 || Dst.size() != 1)
                    throwVerifyError("COPY operation without immediate must have 1 source and 1 destination");

                if (Src[0].Type != Dst[0].Type)
                    throwVerifyError("COPY operation must have same source and destination types");
            }
            break;
        case Opcodes::NEG:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 1 || Dst.size() != 1)
                throwVerifyError("Unary operation must have 1 source and 1 destination");

            if (Src[0].Type != Dst[0].Type)
                throwVerifyError("NEG operation must have same source and destination types");
            break;

        case Opcodes::ADD:
        case Opcodes::SUB:
        case Opcodes::MUL:
        case Opcodes::DIV:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 2 || Dst.size() != 1)
                throwVerifyError("Binary operation must have 2 sources and 1 destination");

            for (auto Source: Src)
                if (!Source.Type.isArithmCompatible(Dst[0].Type))
                    throwVerifyError("Binary operation sources types must be same with dst type");
            break;
        case Opcodes::REM:
        case Opcodes::SHL:
        case Opcodes::SHR:
        case Opcodes::AND:
        case Opcodes::OR:
        case Opcodes::XOR:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 2 || Dst.size() != 1)
                throwVerifyError("Binary operation must have 2 sources and 1 destination");

            if (std::holds_alternative<OpInt>(Dst[0].Type))
                throwVerifyError("This operation is Int only");

            for (auto Source: Src)
                if (!Source.Type.isArithmCompatible(Dst[0].Type))
                    throwVerifyError("Binary operation sources types must be same with dst type");
            break;

        case Opcodes::RET: {
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoDst();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() >= 2)
                throwVerifyError("RET operation must have no or 1 source");

            const auto& RetType = getParent()->getParentFunction()->getRetType();
            if (((Src.size() == 1) != RetType.has_value()) ||
                (RetType.has_value() && Src[0].Type != *RetType))
                throwVerifyError("RET type mismatch");

            break;
        }
        case Opcodes::BR:
            verifyNoImmediate();
            verifyNoDst();
            verifyNoFunc();

            if (Src.size() == 0) {
                verifyNoCmpType();

                if (BrDstBB.size() != 1)
                    throwVerifyError("BR operation with no Src must have 1 BrDstBB");
            } else if (Src.size() == 2) {
                if (!CmpType.has_value())
                    throwVerifyError("BR operation with 2 sources must have CmpType");

                if (Src[0].Type != Src[1].Type)
                    throwVerifyError("BR operation with CmpType must have same type Src");

                if (BrDstBB.size() != 2)
                    throwVerifyError("BR operation with CmpType must have 2 BrDstBB");
            } else {
                throwVerifyError("BR operation must have 1 or 2 BrDstBB");
            }
            break;

        case Opcodes::LOAD:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 1 || Dst.size() != 1)
                throwVerifyError("LOAD operation must have 1 source and 1 destination");

            if (!Src[0].Type.isInt())
                throwVerifyError("LOAD operation Src[0] must be array or int - it's address");
            break;

        case Opcodes::STORE:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 2 || Dst.size() != 0)
                throwVerifyError("STORE operation must have 2 sources and no destination");

            if (!Src[0].Type.isInt())
                throwVerifyError("STORE operation Src[0] must be Int -- it's address");
            break;

        case Opcodes::CALL: {
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();

            if (Dst.size() >= 2)
                throwVerifyError("CALL operation must habe no or 1 destination");

            if (!CallFunc.has_value())
                throwVerifyError("CALL operation must have CallFunc");

            const auto& Args = CallFunc.value()->getArgs();
            if (Src.size() != Args.size())
                throw Instruction::verify_error("CALL Src size must be equal to function argument count");

            auto SrcIt = Src.cbegin() + 1;
            auto ArgIt = Args.cbegin();
            for (; ArgIt != Args.end(); SrcIt++, ArgIt++)
                if (SrcIt->Type != *ArgIt)
                    throw Instruction::verify_error("Func arg type mismatch");

            break;
        }
        case Opcodes::PHI:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();

            if (Dst.size() != 1 || Src.size() != 2)
                throwVerifyError("PHI operation must have 1 destination and 2 sources");

            for (auto Arg: Src) {
                if (Arg.Type != Dst[0].Type)
                    throwVerifyError("PHI operation's sources must have same type with destination");

                if (!Arg.DefBlock.has_value())
                    throwVerifyError("PHI operation's sources must have DefBlock defined");
            }
            break;

        case Opcodes::FUNC_DEF: {
            verifyNoCmpType();
            verifyNoSrc();
            verifyNoBrDstBB();
            verifyNoFunc();
            verifyNoImmediate();
            if (Dst.size() < 1 || !std::holds_alternative<OpInt>(Dst[0].Type))
                throwVerifyError("FUNC_DEF must have at least 1 Dst and Dst[0] must be Int");

            auto& Args = getParent()->getParentFunction()->getArgs();
            if (Dst.size() != Args.size() + 1)
                throw Instruction::verify_error("FUNC_DEF Dst size must be equal to function argument count + 1");

            auto DstIt = Dst.cbegin() + 1;
            auto ArgIt = Args.cbegin();
            for (; ArgIt != Args.end(); DstIt++, ArgIt++)
                if (DstIt->Type != *ArgIt)
                    throw Instruction::verify_error("Func definition arg type mismatch");
            break;
        }
        default:
            unreachable("Unhandled operation");
    }
}

