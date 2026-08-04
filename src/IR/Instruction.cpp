#include "Instruction.h"

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

                if ((std::holds_alternative<int>(*Immediate) && Dst[0].Type != VirtRegister::Int) ||
                    (std::holds_alternative<float>(*Immediate) && Dst[0].Type != VirtRegister::Float) ||
                    (std::holds_alternative<std::string>(*Immediate) && Dst[0].Type != VirtRegister::Int))
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
                if (Source.Type != Dst[0].Type)
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

            if (Dst[0].Type != VirtRegister::Int)
                throwVerifyError("This operation is Int only");

            for (auto Source: Src)
                if (Source.Type != Dst[0].Type)
                    throwVerifyError("Binary operation sources types must be same with dst type");
            break;

        case Opcodes::RET:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoDst();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() >= 2)
                throwVerifyError("RET operation must have no or 1 source");
            break;

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

            if (Src[0].Type != VirtRegister::Int)
                throwVerifyError("LOAD operation Src[0] must be Int -- it's address");
            break;

        case Opcodes::STORE:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Src.size() != 2 || Dst.size() != 0)
                throwVerifyError("STORE operation must have 2 sources and no destination");

            if (Src[0].Type != VirtRegister::Int)
                throwVerifyError("STORE operation Src[0] must be Int -- it's address");
            break;

        case Opcodes::CALL:
            verifyNoImmediate();
            verifyNoCmpType();
            verifyNoBrDstBB();

            if (Dst.size() >= 2)
                throwVerifyError("CALL operation must habe no or 1 destination");

            if (!CallFunc.has_value())
                throwVerifyError("CALL operation must have CallFunc");
            break;

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

        case Opcodes::FUNC_DEF:
            verifyNoCmpType();
            verifyNoSrc();
            verifyNoBrDstBB();
            verifyNoFunc();
            if (Dst.size() < 1)
                throwVerifyError("FUNC_DEF must have at least 1 Dst");

            if (!Immediate.has_value() || !std::holds_alternative<int>(*Immediate) ||
                *std::get_if<int>(&*Immediate) < 0)
                throwVerifyError("FUNC_DEF operation must have non-negative Int immediate with stack frame size");
            break;

        default:
            assert(0);
    }
}

