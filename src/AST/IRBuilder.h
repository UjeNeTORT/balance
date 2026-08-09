#ifndef AST_IR_BUILDER_H_
#define AST_IR_BUILDER_H_

#include "AST/Ast.h"
#include "AST/Node.h"
#include "AST/Visitor.h"
#include "IR/IR.h"
#include "IR/Operand.h"

#include <cassert>
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Balance {

namespace AST {

class Variables {
public:
    Variables(IR& IrRef) : Ir(IrRef) {}

    struct Variable {
        std::variant<size_t, GlobalData*> Id;
        OpType Type;
        bool isConst;

        bool isGlobal() const { return std::holds_alternative<GlobalData*>(Id); }
        VirtRegister getReg() const {
            return {Type, static_cast<int>(*std::get_if<size_t>(&Id)), std::nullopt};
        }
        size_t getId() const {
            assert(!Type.isArray() && !isGlobal());
            return *std::get_if<size_t>(&Id);
        }
        size_t getOffset() const {
            assert(Type.isArray() && !isGlobal());
            return *std::get_if<size_t>(&Id);
        }
        GlobalData* getGData() const {
            assert(isGlobal());
            return *std::get_if<GlobalData*>(&Id);
        }
    };

    auto enterScope() { return Scopes.insert(Scopes.end(), {}); }
    void leaveScope() { Scopes.pop_back(); }

    Variable& addVar(std::string Name, OpType Type, bool IsConst = false);
    Variable& addArray(std::string Name, OpType Type,
                       Function& Func, bool isConst = false);

    Variable& findVar(std::string Name);

private:
    std::vector<std::map<std::string, Variable>> Scopes;

    size_t VarCount = 0;

    IR& Ir;
};

class VariablesScoped {
public:
    VariablesScoped(Variables& VarsRef)
        : Vars(VarsRef)
        , Scope(Vars.enterScope())
    {}
    ~VariablesScoped() { Vars.leaveScope(); }

    auto getScopeVars() const { return Scope; }

private:
    Variables& Vars;
    const std::vector<std::map<std::string, Variables::Variable>>::iterator Scope;
};

class IRBuilder final: public AST::Visitor {
public:
    explicit IRBuilder();

    IR build(const Ast& Tree)&& {
        Tree.getCompUnit()->accept(*this);
        return std::move(Ir);
    }

    virtual void visit(const CompUnitNode& node) override;
    virtual void visit(const VarDeclNode& node) override;
    virtual void visit(const VarDefNode& node) override;
    virtual void visit(const InitValNode& node) override;
    virtual void visit(const FuncDefNode& node) override;
    virtual void visit(const FuncParamNode& node) override;
    virtual void visit(const BlockNode& node) override;
    virtual void visit(const AssignNode& node) override;
    virtual void visit(const ExprStmtNode& node) override;
    virtual void visit(const IfNode& node) override;
    virtual void visit(const WhileNode& node) override;
    virtual void visit(const BreakNode& node) override;
    virtual void visit(const ContinueNode& node) override;
    virtual void visit(const ReturnNode& node) override;
    virtual void visit(const BinaryOpNode& node) override;
    virtual void visit(const UnaryOpNode& node) override;
    virtual void visit(const CallNode& node) override;
    virtual void visit(const LValNode& node) override;
    virtual void visit(const IntLiteralNode& node) override;
    virtual void visit(const FloatLiteralNode& node) override;
private:
    void binaryOpVisitCondition(const BinaryOpNode& node);
    void binaryOpVisitExpression(const BinaryOpNode& node);

    Variables Vars;

    IR Ir;

    VirtRegister convertRegType(OpType NewType, VirtRegister Src);
    void convertRegType(VirtRegister Dst, VirtRegister Src);

    std::optional<VirtRegister> ExprRes;
    bool ExprResRequired = false;
    VirtRegister evalExpr(const Node* Expr) {
        assert(!ExprRes.has_value());
        assert(!ExprResRequired);
        ExprResRequired = true;

        Expr->accept(*this);

        assert(ExprRes.has_value());
        assert(ExprResRequired);
        ExprResRequired = false;
        return *std::exchange(ExprRes, std::nullopt);
    }

    using BrDstIt = std::vector<BasicBlock*>::iterator;
    struct ConditionRes {
        BasicBlock* CondBB;
        std::vector<BrDstIt> FalsePaths;
        std::vector<BrDstIt> TruePaths;
    };
    std::optional<ConditionRes> CondRes;
    bool CondResRequired = false;
    ConditionRes evalCond(const Node* Expr) {
        assert(!CondRes.has_value());
        assert(!ExprResRequired);
        assert(!CondResRequired);
        ExprResRequired = true;
        CondResRequired = true;

        Expr->accept(*this);

        if (ExprRes.has_value()) {
            auto Instr = Ir.addInstruction(Opcodes::BR).setCmpType(CmpTypes::NE)
                                                       .addSrc(*std::exchange(ExprRes, std::nullopt))
                                                       .addSrc(copyImmToSpecType(ExprRes->Type, 0));
            CondRes = {Instr.getParent(), {Instr.addEmptyBrDst()}, {Instr.addEmptyBrDst()}};
        }
        assert(CondRes.has_value());
        assert(ExprResRequired);
        assert(CondResRequired);
        CondResRequired = false;
        ExprResRequired = false;
        return *std::exchange(CondRes, std::nullopt);
    }

    struct InitializerTraversalData {
        const Variables::Variable* Var;
        size_t Depth = 0;
        size_t CurrentOffset = 0;
    };
    std::optional<InitializerTraversalData> InitTraversalData;
    void initializerTraversal(const Variables::Variable& Var, const InitValNode* Node) {
        assert(!InitTraversalData.has_value());
        InitTraversalData = InitializerTraversalData{&Var};
        Node->accept(*this);
        InitTraversalData = std::nullopt;
    }

    auto curFunc() { return Ir.getLastFunction(); }

    template<typename T>
    VirtRegister copyImm(T Val) {
        VirtRegister Reg;
        if constexpr (std::is_same_v<T, int>)
            Reg = Ir.getNewVirtReg(OpInt());
        else if constexpr (std::is_same_v<T, float>)
            Reg = Ir.getNewVirtReg(OpFloat());
        else
            static_assert(0, "unhandled type");
        Ir.addInstruction(Opcodes::COPY).addDst(Reg).setImmediate(Val);
        return Reg;
    }
    template<typename ImmT>
    VirtRegister copyImmToSpecType(OpType Type, ImmT Val) {
        VirtRegister Reg = Ir.getNewVirtReg(Type);
        std::visit(overloaded {
               [this, Reg, Val](OpInt&) {
                Ir.addInstruction(Opcodes::COPY).addDst(Reg).setImmediate((int)Val);
            }, [this, Reg, Val](OpFloat&) {
                Ir.addInstruction(Opcodes::COPY).addDst(Reg).setImmediate((float)Val);
            }, [](auto&) { assert(0 && "for plain types only"); }
        }, Type);
        return Reg;
    }

    VirtRegister evalArrayOffset(const Variables::Variable& Var, const std::vector<ExprPtr>& Indices);

    struct IfFixups {
        std::vector<BrDstIt> Then;
        std::vector<BrDstIt> Else;
        std::vector<BrDstIt> Fin;
    };
    std::vector<IfFixups> IfFixupsStack;

    struct LoopFixups {
        std::vector<BrDstIt> Body;
        std::vector<BrDstIt> Condition;
        std::vector<BrDstIt> Fin;
    };
    std::vector<LoopFixups> LoopFixupsStack;

    void applyFixups(std::vector<BrDstIt>&& Vec) {
        for (const auto& Fixup: Vec)
            *Fixup = &*std::prev(curFunc()->end());
    }
    void applyFixups(std::vector<BrDstIt>&& Vec, BasicBlock* BB) {
        for (const auto& Fixup: Vec)
            *Fixup = BB;
    }

};

} // namespace AST

} // namespace Balance

#endif // AST_IR_BUILDER_H_
