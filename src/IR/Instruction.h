#ifndef IR_INSTRUCTION_H_
#define IR_INSTRUCTION_H_

#include "IR/Operand.h"

#include <cassert>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace Balance {

enum class Opcodes {
    // Base
    NOP,
    // Unary
    CONVERT, BITCAST, COPY, NEG,
    // Binary
    ADD, SUB, MUL, DIV, REM, SHL, SHR, AND, OR, XOR,

    RET,
    BR,
    LOAD,  // Src[0] - address
    STORE, // Src[0] - address, Src[1] - value
    CALL,
    PHI,
    FUNC_DEF,
};

enum class CmpTypes {
    EQ, NE, GT, GE, LT, LE,
};

struct SourceInfo {
    size_t line;
    size_t symbol;
};

class BasicBlock;
class Function;

class Instruction {
public:
    Instruction(Opcodes Op, BasicBlock* Parent, std::optional<SourceInfo> SrcInf = std::nullopt) :
        Opcode(Op), ParentBB(Parent), SrcInfo(SrcInf) {}

    Instruction& addSrc(VirtRegister Reg) { Src.push_back(Reg); return *this; }
    Instruction& addDst(VirtRegister Reg) { Dst.push_back(Reg); return *this; }
    Instruction& setImmediate(ImmVariant Imm) { Immediate = Imm; return *this; }
    Instruction& setCmpType(CmpTypes Type) { CmpType = Type; return *this; }
    Instruction& addBrDst(BasicBlock* Dst) { BrDstBB.push_back(Dst); return *this; }
    Instruction& setCallFunc(Function* Funct) { CallFunc = Funct; return *this; }

    std::pair<std::vector<BasicBlock*>*, size_t> addEmptyBrDst() {
        BrDstBB.push_back(nullptr);
        return {&BrDstBB, BrDstBB.size() - 1};
    }

    void verify() const;

    bool isTerminal() const {
        return (Opcode == Opcodes::BR || Opcode == Opcodes::RET);
    }

    const std::vector<VirtRegister>& getSrc() const { return Src; }
    const std::vector<VirtRegister>& getDst() const { return Dst; }
    const std::vector<BasicBlock*>& getBrDstBB() const { return BrDstBB; }
    std::optional<Function*> getCallFunc() const { return CallFunc; }
    std::optional<ImmVariant> getImm() const { return Immediate; }
    std::optional<CmpTypes> getCmpType() const { return CmpType; }

    Opcodes getOpcode() const { return Opcode; }
    BasicBlock* getParent() const { return ParentBB; }
    std::string getComment() const { return Comment; }

    struct verify_error: public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

private:
    Opcodes Opcode;
    BasicBlock* ParentBB;
    std::string Comment;
    std::optional<SourceInfo> SrcInfo;

    std::optional<ImmVariant> Immediate;
    std::optional<CmpTypes> CmpType;
    std::vector<VirtRegister> Src;
    std::vector<VirtRegister> Dst;
    std::vector<BasicBlock*> BrDstBB;
    std::optional<Function*> CallFunc;

    void throwVerifyError(std::string error) const;

    void verifyNoImmediate() const {
        if (Immediate.has_value()) throwVerifyError("Immediate has value");
    }
    void verifyNoCmpType() const {
        if (CmpType.has_value()) throwVerifyError("CmpType has value");
    }
    void verifyNoSrc() const {
        if (Src.size() != 0) throwVerifyError("Src.size != 0");
    }
    void verifyNoDst() const {
        if (Dst.size() != 0) throwVerifyError("Dst.size != 0");
    }
    void verifyNoBrDstBB() const {
        if (BrDstBB.size() != 0) throwVerifyError("BrDstBB has value");
    }
    void verifyNoFunc() const {
        if (CallFunc.has_value()) throwVerifyError("Func has value");
    }
};

} // Balance

#endif // IR_INSTRUCTION_H_
