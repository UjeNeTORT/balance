#ifndef AST_IR_BUILDER_H
#define AST_IR_BUILDER_H

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
        std::variant<int, size_t, GlobalData*> Id;
        OpType Type;
        bool isConst;

        bool isGlobal() const { return std::holds_alternative<GlobalData*>(Id); }
        VirtRegister getReg() const {
            assert(!Type.isArray() && !isGlobal());
            return {Type, getId(), std::nullopt};
        }
        int getId() const {
            assert(!Type.isArray() && !isGlobal());
            return *std::get_if<int>(&Id);
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

    size_t enterScope() {
        Scopes.push_back({});
        return Scopes.size() - 1;
    }
    void leaveScope() { Scopes.pop_back(); }

    Variable& addVar(std::string Name, OpType Type, bool IsConst = false);
    Variable& addArray(std::string Name, OpType Type,
                       Function& Func, bool isConst = false);

    Variable& findVar(std::string Name);

    const std::vector<std::map<std::string, Variable>>& getScopes() const { return Scopes; }

private:
    std::vector<std::map<std::string, Variable>> Scopes;

    IR& Ir;
};

class VariablesScoped {
public:
    VariablesScoped(Variables& VarsRef) : Vars(VarsRef), ScopeIndex(Vars.enterScope()) {}
    ~VariablesScoped() { Vars.leaveScope(); }

    VariablesScoped(const VariablesScoped&) = delete;
    VariablesScoped& operator=(const VariablesScoped&) = delete;
    VariablesScoped(VariablesScoped&&) = delete;
    VariablesScoped& operator=(VariablesScoped&&) = delete;

    const auto& getScopeVars() const { return Vars.getScopes()[ScopeIndex]; }

private:
    Variables& Vars;
    size_t ScopeIndex;
};

class IRBuilder final: public AST::Visitor {
public:
    explicit IRBuilder();

    IR build(const Ast& Tree) &&;

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

    void buildSuccPred();

    IR Ir;
    Variables Vars;

    VirtRegister convertRegType(OpType NewType, VirtRegister Src);
    void convertRegType(VirtRegister Dst, VirtRegister Src);

    std::optional<VirtRegister> ExprRes;
    bool ExprResRequired = false;
    VirtRegister evalExpr(const Node* Expr) {
        assert(!ExprRes.has_value());
        bool PrevExprResReq = ExprResRequired;
        ExprResRequired = true;

        Expr->accept(*this);

        assert(ExprRes.has_value());
        ExprResRequired = PrevExprResReq;
        return *std::exchange(ExprRes, std::nullopt);
    }

    struct BrDstIt {
        BrDstIt(std::pair<std::vector<BasicBlock*>*, size_t> Iterator): It(Iterator) {}
        std::pair<std::vector<BasicBlock*>*, size_t> It;
        BasicBlock*& operator*() { return (*It.first)[It.second]; }
    };

    struct ConditionRes {
        BasicBlock* CondBB;
        std::vector<BrDstIt> FalsePaths;
        std::vector<BrDstIt> TruePaths;
    };
    std::optional<ConditionRes> CondRes;
    bool CondResRequired = false;
    ConditionRes evalCond(const Node* Expr) {
        assert(!CondRes.has_value());
        bool PrevExprResReq = ExprResRequired;
        bool PrevCondResReq = CondResRequired;
        ExprResRequired = true;
        CondResRequired = true;

        Expr->accept(*this);

        if (ExprRes.has_value()) {
            auto ZeroImm = copyImmToSpecType(ExprRes->Type, 0);
            auto Instr = Ir.addInstruction(Opcodes::BR).setCmpType(CmpTypes::NE)
                                                       .addSrc(*std::exchange(ExprRes, std::nullopt))
                                                       .addSrc(ZeroImm);
            CondRes = {Instr.getParent(), {Instr.addEmptyBrDst()}, {Instr.addEmptyBrDst()}};
        }
        assert(CondRes.has_value());
        CondResRequired = PrevCondResReq;
        ExprResRequired = PrevExprResReq;
        return *std::exchange(CondRes, std::nullopt);
    }

    struct InitializerTraversalData {
        const Variables::Variable* Var;
        size_t Depth = 0;
        size_t CurrentOffset = 0;
    };
    std::optional<InitializerTraversalData> InitTraversalData;
    void initializerTraversal(const Variables::Variable* Var, const InitValNode* Node) {
        assert(!InitTraversalData.has_value());
        InitTraversalData = InitializerTraversalData{Var};
        Node->accept(*this);
        InitTraversalData = std::nullopt;
    }

    Function* curFunc() { return Ir.getLastFunction(); }

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
        for (auto& Fixup: Vec)
            *Fixup = &*std::prev(curFunc()->end());
    }
    void applyFixups(std::vector<BrDstIt>&& Vec, BasicBlock* BB) {
        for (auto& Fixup: Vec)
            *Fixup = BB;
    }
};

} // namespace AST

} // namespace Balance

#endif // AST_IR_BUILDER_H
