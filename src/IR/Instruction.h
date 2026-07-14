#ifndef IR_INSTRUCTION_H_
#define IR_INSTRUCTION_H_

#include "Operand.h"
#include "Utils.h"

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Balance {

namespace Opcodes {
enum Opcodes {
    // Base
    NOP,
    // Unary
    CONVERT, BITCAST, COPY, NEG,
    // Binary
    ADD, SUB, MUL, DIV, REM, SHL, SHR, AND, OR, XOR, CMP,
    // Ret
    RET,
    // Br
    BR,
    // Load
    LOAD,
    // Store
    STORE,
    // Call
    CALL,
    // Phi
    PHI,
};
}

enum class CmpTypes {
    EQ, NE, GT, GE, LT, LE,
};

// TODO: function arguments retrieval

struct SourceInfo {
    size_t line;
    size_t symbol;
};

class BasicBlock;

class Instruction {
public:
    virtual void verify() const;

    virtual bool isInt()      const { return false; }
    virtual bool isFloat()    const { return false; }
    virtual bool isTerminal() const { return false; }


    virtual std::vector<VirtRegister> getArguments() const { return {}; }
    virtual std::optional<VirtRegister> getDestination() const { return std::nullopt; }

    Opcodes::Opcodes getOpcode() const { return Opcode; }
    BasicBlock* getParentBB() const { return ParentBB; }
    std::string getComment() const { return Comment; }

    struct verify_error: public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

protected:
    Opcodes::Opcodes Opcode;
    BasicBlock* ParentBB;
    std::string Comment;
    std::optional<SourceInfo> SourceInfo;
};

class UnaryInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt() const override {
        return std::holds_alternative<IntVirtRegister>(Dst);;
    }
    virtual bool isFloat() const override {
        return std::holds_alternative<FloatVirtRegister>(Dst);
    }

    virtual std::vector<VirtRegister> getArguments() const override {
        return std::visit(overloaded {
            [](IntVirtRegister&& arg)   { return std::vector<VirtRegister>{arg.Id}; },
            [](FloatVirtRegister&& arg) { return std::vector<VirtRegister>{arg.Id}; },
            [](auto&&)                  { return std::vector<VirtRegister>{}; }
        }, Src);
    }
    virtual std::optional<VirtRegister> getDestination() const override {
        return std::visit([](auto&& arg) { return arg.Id; }, Dst);
    }

private:
    std::variant<IntVirtRegister, FloatVirtRegister, int, float> Src;
    std::variant<IntVirtRegister, FloatVirtRegister> Dst;
};

class BinaryInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override { return std::holds_alternative<IntVirtRegister>(Dst); }
    virtual bool isFloat() const override { return std::holds_alternative<FloatVirtRegister>(Dst); }

    virtual std::vector<VirtRegister> getArguments() const override {
        return std::visit([](auto&& arg) { return std::vector<VirtRegister>{arg[0].Id, arg[1].Id}; }, Src);
    }
    virtual std::optional<VirtRegister> getDestination() const override {
        return std::visit([](auto&& arg) { return arg.Id; }, Dst);
    }

private:
    std::variant<std::array<IntVirtRegister, 2>, std::array<FloatVirtRegister, 2>> Src;
    std::variant<IntVirtRegister, FloatVirtRegister> Dst;
    std::optional<CmpTypes> CmpType;
};

class RetInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override {
        return Src.has_value() && std::holds_alternative<IntVirtRegister>(*Src);
    }
    virtual bool isFloat() const override {
        return Src.has_value() && std::holds_alternative<FloatVirtRegister>(*Src);
    }

    virtual std::vector<VirtRegister> getArguments() const override {
        if (Src.has_value())
            return {std::visit([](auto&& arg) { return arg.Id; }, *Src)};
        return {};
    }

private:
    std::optional<std::variant<IntVirtRegister, FloatVirtRegister>> Src;
};

class BrInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt() const override { return true; }

    bool isConditional() const { return Src.has_value(); }

    virtual std::vector<VirtRegister> getArguments() const override {
        if (Src.has_value())
            return { Src->Id };
        return {};
    }

private:
    std::optional<IntVirtRegister> Src;
    BasicBlock* DstBB;
};

class LoadInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override { return std::holds_alternative<IntVirtRegister>(Src); }
    virtual bool isFloat() const override { return std::holds_alternative<FloatVirtRegister>(Src); }

    virtual std::vector<VirtRegister> getArguments() const override {
        return {std::visit([](auto&& arg) { return arg.Id; }, Src), Address.Id};
    }

private:
    std::variant<IntVirtRegister, FloatVirtRegister> Src;
    IntVirtRegister Address;
};

class StoreInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override { return std::holds_alternative<IntVirtRegister>(Dst); }
    virtual bool isFloat() const override { return std::holds_alternative<FloatVirtRegister>(Dst); }

    virtual std::vector<VirtRegister> getArguments() const override {
        return {Address.Id};
    }

    virtual std::optional<VirtRegister> getDestination() const override {
        return std::visit([](auto&& arg) { return arg.Id; }, Dst);
    }

private:
    std::variant<IntVirtRegister, FloatVirtRegister> Dst;
    IntVirtRegister Address;
};

class CallInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override {
        return Dst.has_value() && std::holds_alternative<IntVirtRegister>(*Dst);
    }
    virtual bool isFloat() const override {
        return Dst.has_value() && std::holds_alternative<FloatVirtRegister>(*Dst);
    }

    virtual std::vector<VirtRegister> getArguments() const override {
        std::vector<VirtRegister> v;
        for (auto arg: Arguments)
            v.push_back(std::visit([](auto&& a) { return a.Id; }, arg));
        return v;
    }

    virtual std::optional<VirtRegister> getDestination() const override {
        if (Dst.has_value())
            return std::visit([](auto&& arg) { return arg.Id; }, *Dst);
        return std::nullopt;
    }

private:
    std::vector<std::variant<IntVirtRegister, FloatVirtRegister>> Arguments;
    std::optional<std::variant<IntVirtRegister, FloatVirtRegister>> Dst;
    BasicBlock* FunctionBB;
};

class PhiInstruction: public Instruction {
public:
    virtual void verify() const override;

    virtual bool isInt()   const override {
        return std::holds_alternative<IntVirtRegister>(Dst);
    }
    virtual bool isFloat() const override {
        return std::holds_alternative<FloatVirtRegister>(Dst);
    }

    virtual std::vector<VirtRegister> getArguments() const override {
        std::vector<VirtRegister> v;
        for (auto arg: Arguments)
            v.push_back(std::visit([](auto&& a) { return a.Id; }, arg));
        return v;
    }

    virtual std::optional<VirtRegister> getDestination() const override {
        return std::visit([](auto&& arg) { return arg.Id; }, Dst);
    }

private:
    std::vector<std::variant<IntVirtRegister, FloatVirtRegister>> Arguments;
    std::variant<IntVirtRegister, FloatVirtRegister> Dst;
};

} // Balance

#endif // IR_INSTRUCTION_H_
