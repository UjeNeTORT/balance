#ifndef IR_INSTRUCTION_H_
#define IR_INSTRUCTION_H_

#include "Operand.h"

#include <cassert>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace Balance {

enum class Opcodes {
    // Base
    NOP,
    // Unary
    CONVERT, BITCAST, COPY, NEG,
    // Binary
    ADD, SUB, MUL, DIV, REM, SHL, SHR, AND, OR, XOR, CMP,

    RET,
    BR,
    LOAD,  // Src[0] - address
    STORE, // Src[0] - address, Src[1] - value
    CALL,
    PHI,
    FUNC_DEF,
    ALLOCA,
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
    Instruction(Opcodes Op, BasicBlock* Parent, std::optional<SourceInfo> SrcInf = std::nullopt)
        : Opcode(Op)
        , ParentBB(Parent)
        , SrcInfo(SrcInf)
    {}

    void addSrc(VirtRegister Reg) { Src.push_back(Reg); }
    void addDst(VirtRegister Reg) { Dst.push_back(Reg); }
    void setImmediate(std::variant<int, float> Imm) { Immediate = Imm; }
    void setCmpType(CmpTypes Type) { CmpType = Type; }
    void addBrDst(BasicBlock* Dst) { BrDstBB.push_back(Dst); }
    void setCallFunc(Function* Funct) { CallFunc = Funct; }

    void verify() const;

    bool isTerminal() const {
        return (Opcode == Opcodes::BR || Opcode == Opcodes::RET);
    }

    const std::vector<VirtRegister>& getSrc() const { return Src; }
    const std::vector<VirtRegister>& getDst() const { return Dst; }
    const std::vector<BasicBlock*>& getBrDstBB() const { return BrDstBB; }
    std::optional<Function*> getCallFunc() const { return CallFunc; }
    std::optional<std::variant<int, float>> getImm() const { return Immediate; }
    std::optional<CmpTypes> getCmpType() const { return CmpType; }

    const std::vector<BasicBlock*>& getBrDstBB() const { return BrDstBB; }
    std::optional<Function*> getCallFunc() const { return CallFunc; }
    std::optional<std::variant<int, float>> getImm() const { return Immediate; }
    std::optional<CmpTypes> getCmpType() const { return CmpType; }

    Opcodes getOpcode() const { return Opcode; }
    BasicBlock* getParentBB() const { return ParentBB; }
    std::string getComment() const { return Comment; }

    struct verify_error: public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

private:
    Opcodes Opcode;
    BasicBlock* ParentBB;
    std::string Comment;
    std::optional<SourceInfo> SrcInfo;

    std::optional<std::variant<int, float>> Immediate;
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
        if (BrDstBB.size() != 0) throwVerifyError("BrDstBB hsa value");
    }
    void verifyNoFunc() const {
        if (CallFunc.has_value()) throwVerifyError("Func has value");
    }
};

} // Balance

#endif // IR_INSTRUCTION_H_
