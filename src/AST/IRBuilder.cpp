#include "AST/IRBuilder.h"

#include "AST/ConstantFolding.h"
#include "AST/Node.h"
#include "Utils/Utils.h"
#include "IR/Operand.h"
#include "IR/Function.h"
#include "IR/BasicBlock.h"
#include "IR/Instruction.h"

#include <array>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>

using namespace Balance;
using namespace AST;

namespace {

template<typename T>
T getImmediateAndConvert(std::variant<int, float> Value) {
    return std::visit([](auto& Val) { return (T)Val; }, Value);
}

auto parseArrayDims(std::vector<ExprPtr> Dims, bool IsFuncParam = false) {
    std::vector<size_t> VarDims;
    if (IsFuncParam)
        VarDims.push_back(0);

    for (auto* Dim: Dims) {
        auto DimVal = getConstVal(Dim);
        if (!DimVal.has_value())
            throw std::runtime_error("Array dimensions must be constant expressions");

        int Dimension = getImmediateAndConvert<int>(*DimVal);
        if (Dimension <= 0)
            throw std::runtime_error("Array dimensions must be > 0");

        VarDims.push_back(Dimension);
    }
    return VarDims;
}

OpType convertBaseTypeForVar(BaseType Type) {
    switch (Type) {
        case BaseType::Int:   return OpInt();
        case BaseType::Float: return OpFloat();
        case BaseType::Void:  throw std::runtime_error("Variable can't have void type");
        default: unreachable("unhandled type");
    }
}

OpType convertBaseTypeForArray(BaseType Type, std::vector<size_t>&& Dims) {
    switch (Type) {
        case BaseType::Int:   return OpArray<OpInt>(Dims);
        case BaseType::Float: return OpArray<OpFloat>(Dims);
        case BaseType::Void:  throw std::runtime_error("Variable can't have void type");
        default: unreachable("unhandled type");
    }
}

} // anonymous namespace

Variables::Variable& Variables::addVar(std::string Name, OpType Type, bool IsConst) {
    Variable Var;
    if (Scopes.size() == 1)
        Var = Variable{&*Ir.addGlobalData(GlobalData(Name, IsConst, {})), Type, IsConst};
    else
        Var = Variable{VarCount++, Type, IsConst};

    auto [It, Succ] = std::prev(Scopes.end())->emplace(Name, std::move(Var));
    if (!Succ)
        throw std::runtime_error("Redefenition of variable");

    return It->second;
}

Variables::Variable& Variables::addArray(std::string Name, OpType Type,
                                         Function& Func, bool IsConst) {
    Variable Var;
    if (Scopes.size() == 1)
        Var = Variable{&*Ir.addGlobalData(GlobalData(Name, IsConst, {})), Type, IsConst};
    else
        Var = Variable{Func.getFrameSize(), Type, IsConst};

    auto [It, Succ] = std::prev(Scopes.end())->emplace(Name, std::move(Var));
    if (!Succ)
        throw std::runtime_error("Redefenition of variable");

    if (!Var.isGlobal())
        Func.incFrameSize(Type.getSize());

    return It->second;
}

Variables::Variable& Variables::findVar(std::string Name) {
    assert(Scopes.begin() != Scopes.end());

    auto It = Scopes.end();

    do {
        It = std::prev(It);

        auto MapIt = It->find(Name);
        if (MapIt != It->end())
            return MapIt->second;

    } while (It != Scopes.begin());

    throw std::runtime_error("Usage of undeclared variable");
}

IRBuilder::IRBuilder(): Vars(Ir) {
    Ir.addFunction(IR::internalFunc("memset"));

    auto Addr = Ir.getNewVirtReg(OpInt());
    auto Val  = Ir.getNewVirtReg(OpInt());
    auto Len  = Ir.getNewVirtReg(OpInt());
    Ir.addInstruction(Opcodes::FUNC_DEF).addDst(Ir.getNewVirtReg(OpInt()))
                                        .addDst(Addr).addDst(Val).addDst(Len);
    auto FinAddr = Ir.getNewVirtReg(OpInt());
    Ir.addInstruction(Opcodes::ADD).addDst(FinAddr).addSrc(Addr).addSrc(Len);

    auto& IfBr = Ir.addInstruction(Opcodes::BR).setCmpType(CmpTypes::LT).addSrc(Addr).addSrc(FinAddr);
    auto BodyBB = Ir.addInstruction(Opcodes::STORE).addSrc(Addr).addSrc(Val).getParent();
    Ir.addInstruction(Opcodes::ADD).addDst(Addr).addSrc(Addr).addSrc(copyImm(1));

    auto& WhileBr = Ir.addInstruction(Opcodes::BR).setCmpType(CmpTypes::LT).addSrc(Addr).addSrc(FinAddr);

    auto FinBB = Ir.addInstruction(Opcodes::RET).getParent();

    IfBr.addBrDst(FinBB).addBrDst(BodyBB);
    WhileBr.addBrDst(FinBB).addBrDst(BodyBB);

    // TODO: add runtime functions
}

VirtRegister IRBuilder::convertRegType(OpType NewType, VirtRegister Src) {
    if (Src.Type.isArray() != NewType.isArray())
        throw std::runtime_error("Can't convert non-array type from/to array type");

    if (Src.Type == NewType)
        return Src;

    if (Src.Type.isArray())
        throw std::runtime_error("Can't convert array to a different type array");

    auto Dst = Ir.getNewVirtReg(NewType);
    Ir.addInstruction(Opcodes::CONVERT).addDst(Dst).addSrc(Src);
    return Dst;
}

void IRBuilder::convertRegType(VirtRegister Dst, VirtRegister Src) {
    if (Src.Type.isArray() != Dst.Type.isArray())
        throw std::runtime_error("Can't convert non-array type from/to array type");

    if (Src.Type == Dst.Type)
        Ir.addInstruction(Opcodes::COPY).addDst(Dst).addSrc(Src);

    if (Src.Type.isArray())
        throw std::runtime_error("Can't convert array to a different type array");

    Ir.addInstruction(Opcodes::CONVERT).addDst(Dst).addSrc(Src);
}

VirtRegister IRBuilder::evalArrayOffset(const Variables::Variable& Var, const std::vector<ExprPtr>& Indices) {
    auto OffsetReg = Ir.getNewVirtReg(Var.Type.isArray() ? Var.Type : Var.Type.makeArray({0}));
    if (Var.isGlobal())
        Ir.addInstruction(Opcodes::COPY).addDst(OffsetReg).setImmediate(Var.getGData());
    else
        Ir.addInstruction(Opcodes::ADD).addDst(OffsetReg).addSrc(curFunc()->getFrameVReg())
                                                         .addSrc(copyImm((int)Var.getOffset()));

    if (Indices.size() > Var.Type.getDepth())
        throw std::runtime_error("Too many indices for array");

    for (const auto* Indice: Indices) {
        auto Regs = Ir.getNewVirtRegs<2>(OffsetReg.Type.getSubdimType(1));

        Ir.addInstruction(Opcodes::MUL).addDst(Regs[0])
                                       .addSrc(evalExpr(Indice))
                                       .addSrc(copyImm((int)OffsetReg.Type.getSubdimSize(1)));

        Ir.addInstruction(Opcodes::ADD).addDst(Regs[1]).addSrc(OffsetReg).addSrc(Regs[0]);
        OffsetReg = Regs[1];
    }
    return OffsetReg;
}

void IRBuilder::visit(const CompUnitNode& node) {
    VariablesScoped Scope(Vars);

    for (const auto* Item: node.getItems())
        Item->accept(*this);
}

void IRBuilder::visit(const FuncDefNode& node) {
    VariablesScoped Scope(Vars);

    auto Func = Ir.addFunction(Function(node.getName()));

    switch (node.getReturnType()) {
        case BaseType::Int:   Func->setRetType(OpInt()); break;
        case BaseType::Float: Func->setRetType(OpFloat()); break;
        case BaseType::Void:  Func->setRetType(std::nullopt); break;
        default: unreachable("unhandled type");
    }

    for (const auto* Param: node.getParams())
        Param->accept(*this);

    auto& FuncDef = Ir.addInstruction(Opcodes::FUNC_DEF).addDst(Func->getNewVirtReg(OpInt()));

    for (auto& Var: Scope.getScopeVars())
        FuncDef.addDst(Var.second.getReg());

    node.getBody()->accept(*this);

    if (!Func->getRetType().has_value())
        Ir.addInstruction(Opcodes::RET);
}

void IRBuilder::visit(const FuncParamNode& node) {
    OpType Type;
    if (node.isArray())
        Type = convertBaseTypeForArray(node.getType(), parseArrayDims(node.getDims(), true));
    else
        Type = convertBaseTypeForVar(node.getType());

    Vars.addVar(node.getName(), Type);
    curFunc()->addArg(Type);
}

void IRBuilder::visit(const VarDeclNode& node) {
    for (auto* Def: node.getDefs()) {
        Variables::Variable Var;
        if (Def->getDims().size() != 0) {
            Var = Vars.addArray(Def->getName(), convertBaseTypeForArray(node.getType(),
                                                parseArrayDims(Def->getDims())), *curFunc(),
                                node.isConst());
        } else {
            Var = Vars.addVar(Def->getName(), convertBaseTypeForVar(node.getType()), node.isConst());
        }

        if (Def->hasInit())
            initializerTraversal(Var, Def->getInit());
    }
}

void IRBuilder::visit(const VarDefNode&) {
    assert(0 && "We mustn't visit VarDefNode. It is handled in VarDeclNode");
}

void IRBuilder::visit(const InitValNode& node) {
    assert(InitTraversalData.has_value());

    auto& Var = *InitTraversalData->Var;
    auto& Depth = InitTraversalData->Depth;
    auto& CurOffset = InitTraversalData->CurrentOffset;

    if (Depth == 0 && node.isList() != Var.Type.isArray())
        throw std::runtime_error("Array variable must have list-style initializer");

    if (!Var.Type.isArray()) {
        assert(Depth == 0);

        if (Var.isGlobal()) {
            Var.getGData()->addInitVal(AstConstantFolder().foldConstInit(node.getExpr(), &Vars));
            return;
        }
        convertRegType(Var.getReg(), evalExpr(node.getExpr()));
        return;
    }

    if (Depth == 0) {
        assert(CurOffset == 0);

        if (Var.isGlobal()) {
            Var.getGData()->setInitVals(std::vector<int>(0, Var.Type.getSize() / Var.Type.getSize()));
        } else {
            auto Reg = Ir.getNewVirtReg(OpInt());

            Ir.addInstruction(Opcodes::ADD ).addDst(Reg).addSrc(curFunc()->getFrameVReg())
                                                        .addSrc(copyImm((int)(CurOffset + Var.getOffset())));

            Ir.addInstruction(Opcodes::CALL).setCallFunc(&*Ir.findFunction(IR::internalFunc("memset")))
                                            .addSrc(Reg).addSrc(copyImm(0)).addSrc(copyImm((int)Var.Type.getSize()));
        }
    }

    if (node.isList()) {
        size_t DepthOffset = CurOffset;

        Depth++;
        for (const auto* ValNode: node.getList())
            ValNode->accept(*this);
        Depth--;

        CurOffset = DepthOffset + Var.Type.getSubdimSize(Depth);

        if (Depth == 0)
            assert(CurOffset == Var.Type.getSize());
        return;
    }

    if (!Var.isGlobal()) {
        auto Res = evalExpr(node.getExpr());
        std::visit([this, &Res](auto& VarType, auto& ResType) {
            using VarT = std::decay_t<decltype(VarType)>;
            using ResT = std::decay_t<decltype(ResType)>;

            if constexpr (VarT::IsArray) {
                if constexpr (!std::is_same_v<typename VarT::array_t, ResT>) {
                    auto NewRes = Ir.getNewVirtReg(VarType);
                    Ir.addInstruction(Opcodes::CONVERT).addDst(NewRes).addSrc(Res);
                    Res = NewRes;
                }
            } else {
                assert(0 && "this code is for arrays only");
            }
        }, Var.Type, Res.Type);

        auto AddrReg = Ir.getNewVirtReg(Var.Type.getSubdimType(Depth));

        Ir.addInstruction(Opcodes::ADD).addDst(AddrReg).addSrc(curFunc()->getFrameVReg())
                                                       .addSrc(copyImm((int)CurOffset));
        Ir.addInstruction(Opcodes::STORE).addSrc(AddrReg).addSrc(Res);
        return;
    }

    auto Value = AstConstantFolder().foldConstInit(node.getExpr(), &Vars);
    std::visit([CurOffset, Var](auto& Init, auto& Val) {
        Init[CurOffset / Var.Type.getSize()] = Val;
    }, Var.getGData()->getInit(), Value);
}

void IRBuilder::visit(const BlockNode& node) {
    VariablesScoped Scope(Vars);

    for (const auto* Item: node.getItems()) {
        Item->accept(*this);
        if (ExprRes.has_value())
            ExprRes = std::nullopt;
    }
}

void IRBuilder::visit(const ExprStmtNode& node) {
    if (node.hasExpr())
        ExprRes = evalExpr(node.getExpr());
}

void IRBuilder::visit(const AssignNode& node) {
    auto Res = evalExpr(node.getSrc());

    auto Dst = node.getDest();
    auto& Var = Vars.findVar(Dst->getName());

    if (Dst->getIndices().size() != Var.Type.getSize())
        throw std::runtime_error("Incompatible dimensions in assignment");

    if (!Var.Type.isArray()) {
        VirtRegister DstReg;
        if (Var.isGlobal())
            DstReg = Ir.getNewVirtReg(Var.Type);
        else
            DstReg = Var.getReg();

        convertRegType(DstReg, Res);

        if (Var.isGlobal()) {
            auto Addr = Ir.getNewVirtReg(DstReg.Type.makeArray({0}));
            Ir.addInstruction(Opcodes::COPY).addDst(Addr).setImmediate(Var.getGData());
            Ir.addInstruction(Opcodes::STORE).addSrc(Addr).addSrc(DstReg);
        }
        return;
    }

    auto OffsetReg = evalArrayOffset(Var, Dst->getIndices());

    Ir.addInstruction(Opcodes::STORE).addSrc(OffsetReg).addSrc(Res);
}

void IRBuilder::visit(const LValNode& node) {
    auto& Var = Vars.findVar(node.getName());

    if (!Var.Type.isArray()) {
        if (node.getIndices().size() != 0)
            throw std::runtime_error("This variable isn't array");

        if (!Var.isGlobal()) {
            ExprRes = Var.getReg();
            return;
        }

        ExprRes = Ir.getNewVirtReg(Var.Type);
        auto Addr = Ir.getNewVirtReg(Var.Type.makeArray({0}));
        Ir.addInstruction(Opcodes::COPY).addDst(Addr).setImmediate(Var.getGData());
        Ir.addInstruction(Opcodes::LOAD).addDst(*ExprRes).addSrc(Addr);
        return;
    }

    auto OffsetReg = evalArrayOffset(Var, node.getIndices());

    ExprRes = Ir.getNewVirtReg(std::visit([](const auto& Type) {
        using T = std::decay_t<decltype(Type)>;
        if constexpr (T::IsArray) {
            return OpType(typename T::array_t());
        } else {
            assert(0 && "array only");
            return OpType();
        }
    }, Var.Type));
    Ir.addInstruction(Opcodes::LOAD).addSrc(OffsetReg);
}

void IRBuilder::visit(const UnaryOpNode& node) {
    if (CondResRequired) {
        assert(ExprResRequired);

        switch (node.getOp()) {
            case UnaryOp::LogicalNot: {
                auto Res = evalCond(node.getOperand());
                std::swap(Res.FalsePaths, Res.TruePaths);
                CondRes = std::move(Res);
                return;
            }

            case UnaryOp::Plus:
            case UnaryOp::Minus:
                break;

            default: unreachable("unhandled op");
        }
    }

    auto Expr = evalExpr(node.getOperand());
    if (Expr.Type.isArray())
        throw std::runtime_error("Unary operations aren't allowed for arrays");

    switch (node.getOp()) {
        case UnaryOp::Plus:
            ExprRes = Expr;
            break;
        case UnaryOp::Minus:
            ExprRes = Ir.getNewVirtReg(Expr.Type);
            Ir.addInstruction(Opcodes::NEG).addDst(*ExprRes).addSrc(Expr);
            break;
        case UnaryOp::LogicalNot:
            throw std::runtime_error("Logical operations aren't allowed in expressions");

        default: unreachable("unhandled op");
    }
}

void IRBuilder::binaryOpVisitCondition(const BinaryOpNode& node) {
    CmpTypes BrCmp;
    switch (node.getOp()) {
        case BinaryOp::Add: 
        case BinaryOp::Sub:
        case BinaryOp::Mul:
        case BinaryOp::Div:
        case BinaryOp::Mod:
            binaryOpVisitExpression(node);
            return;

        case BinaryOp::Less:         BrCmp = CmpTypes::LT; break;
        case BinaryOp::Greater:      BrCmp = CmpTypes::GT; break;
        case BinaryOp::LessEqual:    BrCmp = CmpTypes::LE; break;
        case BinaryOp::GreaterEqual: BrCmp = CmpTypes::GE; break;
        case BinaryOp::Equal:        BrCmp = CmpTypes::EQ; break;
        case BinaryOp::NotEqual:     BrCmp = CmpTypes::NE; break;

        case BinaryOp::LogicalAnd: {
            auto Left = evalCond(node.getLeft());
            auto Right = evalCond(node.getRight());

            applyFixups(std::move(Left.TruePaths), Right.CondBB);

            auto AllFalse = std::move(Left.FalsePaths);
            AllFalse.insert(AllFalse.end(), Right.FalsePaths.begin(), Right.FalsePaths.end());
            CondRes = {Left.CondBB, std::move(AllFalse), std::move(Right.TruePaths)};
            return;
        }
        case BinaryOp::LogicalOr: {
            auto Left = evalCond(node.getLeft());
            auto Right = evalCond(node.getRight());

            applyFixups(std::move(Left.FalsePaths), Right.CondBB);

            auto AllTrue = std::move(Left.TruePaths);
            AllTrue.insert(AllTrue.end(), Right.TruePaths.begin(), Right.TruePaths.end());
            CondRes = {Left.CondBB, std::move(Right.FalsePaths), std::move(AllTrue)};
            return;
        }
        default: unreachable("Unhandled operation");
    }
    auto& Br = Ir.addInstruction(Opcodes::BR).setCmpType(BrCmp)
                                             .addSrc(evalExpr(node.getLeft()))
                                             .addSrc(evalExpr(node.getRight()));
    CondRes = {Br.getParent(), {Br.addEmptyBrDst()}, {Br.addEmptyBrDst()}};

}

void IRBuilder::binaryOpVisitExpression(const BinaryOpNode& node) {
    auto Left  = evalExpr(node.getLeft());
    auto Right = evalExpr(node.getRight());

    if (Left.Type.isArray() || Right.Type.isArray())
        throw std::runtime_error("Binary operations aren't allowed for arrays");

    if (std::holds_alternative<OpFloat>(Left.Type) || std::holds_alternative<OpFloat>(Right.Type)) {
        Left  = convertRegType(OpFloat(), Left);
        Right = convertRegType(OpFloat(), Right);
    }

    switch (node.getOp()) {
        case BinaryOp::Add: 
            ExprRes = Ir.getNewVirtReg(Left.Type);
            Ir.addInstruction(Opcodes::ADD).addDst(*ExprRes).addSrc(Left).addSrc(Right);
            break;
        case BinaryOp::Sub:
            ExprRes = Ir.getNewVirtReg(Left.Type);
            Ir.addInstruction(Opcodes::SUB).addDst(*ExprRes).addSrc(Left).addSrc(Right);
            break;
        case BinaryOp::Mul:
            ExprRes = Ir.getNewVirtReg(Left.Type);
            Ir.addInstruction(Opcodes::MUL).addDst(*ExprRes).addSrc(Left).addSrc(Right);
            break;
        case BinaryOp::Div:
            ExprRes = Ir.getNewVirtReg(Left.Type);
            Ir.addInstruction(Opcodes::DIV).addDst(*ExprRes).addSrc(Left).addSrc(Right);
            break;
        case BinaryOp::Mod:
            if (std::holds_alternative<OpFloat>(Left.Type))
                throw std::runtime_error("Mod operation (%) isn't allowed for float operands");

            ExprRes = Ir.getNewVirtReg(Left.Type);
            Ir.addInstruction(Opcodes::REM).addDst(*ExprRes).addSrc(Left).addSrc(Right);
            break;

        case BinaryOp::Less:
        case BinaryOp::Greater:
        case BinaryOp::LessEqual:
        case BinaryOp::GreaterEqual:
        case BinaryOp::Equal:
        case BinaryOp::NotEqual:
        case BinaryOp::LogicalAnd:
        case BinaryOp::LogicalOr:
            throw std::runtime_error("Logical and comparison operations aren't allowed in expressions");

        default: unreachable("Unhandled operation");
    }
}

void IRBuilder::visit(const BinaryOpNode& node) {
    if (CondResRequired) {
        assert(ExprResRequired);

        binaryOpVisitCondition(node);
        return;
    }

    binaryOpVisitExpression(node);
}

void IRBuilder::visit(const IntLiteralNode& node) {
    ExprRes = Ir.getNewVirtReg(OpInt());
    Ir.addInstruction(Opcodes::COPY).addDst(*ExprRes).setImmediate(node.getValue());
}

void IRBuilder::visit(const FloatLiteralNode& node) {
    ExprRes = Ir.getNewVirtReg(OpInt());
    Ir.addInstruction(Opcodes::COPY).addDst(*ExprRes).setImmediate(node.getValue());
}

void IRBuilder::visit(const CallNode& node) {
    auto Func = Ir.findFunction(node.getCallee());

    auto& Call = Ir.addInstruction(Opcodes::CALL).setCallFunc(&*Func);

    if (node.getArgs().size() != Func->getArgs().size())
        throw std::runtime_error("Call has invalid number of arguments");

    for (size_t i = 0; i < node.getArgs().size(); i++)
        Call.addSrc(convertRegType(Func->getArgs()[i], evalExpr(node.getArgs()[i])));

    if (Func->getRetType().has_value()) {
        ExprRes = Ir.getNewVirtReg(*Func->getRetType());
        Call.addDst(*ExprRes);
    } else if (ExprResRequired) {
        throw std::runtime_error("Void function can't be used in expressions");
    }
}

void IRBuilder::visit(const ReturnNode& node) {
    auto& Ret = Ir.addInstruction(Opcodes::RET);

    if (node.hasExpr() != curFunc()->getRetType().has_value())
        throw std::runtime_error("Invalid return type");

    if (node.hasExpr())
        Ret.addSrc(convertRegType(*curFunc()->getRetType(), evalExpr(node.getExpr())));
}

void IRBuilder::visit(const IfNode& node) {
    auto Cond = evalCond(node.getCond());

    if (node.hasElse())
        IfFixupsStack.push_back({std::move(Cond.TruePaths), std::move(Cond.FalsePaths), {}});
    else
        IfFixupsStack.push_back({std::move(Cond.TruePaths), {}, std::move(Cond.FalsePaths)});

    applyFixups(std::move(std::prev(IfFixupsStack.end())->Then));
    {
        VariablesScoped Scope(Vars);
        node.getThen()->accept(*this);
        if (ExprRes.has_value())
            ExprRes = std::nullopt;
    }

    std::prev(IfFixupsStack.end())->Fin.push_back(Ir.addInstruction(Opcodes::BR).addEmptyBrDst());

    if (node.hasElse()) {
        applyFixups(std::move(std::prev(IfFixupsStack.end())->Else));

        VariablesScoped Scope(Vars);
        node.getThen()->accept(*this);
        if (ExprRes.has_value())
            ExprRes = std::nullopt;

        std::prev(IfFixupsStack.end())->Fin.push_back(Ir.addInstruction(Opcodes::BR).addEmptyBrDst());
    } else {
        assert(std::prev(IfFixupsStack.end())->Else.size() == 0);
    }

    applyFixups(std::move(std::prev(IfFixupsStack.end())->Fin));

    assert(std::prev(IfFixupsStack.end())->Then.size() == 0);
    assert(std::prev(IfFixupsStack.end())->Else.size() == 0);
    assert(std::prev(IfFixupsStack.end())->Fin.size() == 0);

    IfFixupsStack.pop_back();
}

void IRBuilder::visit(const WhileNode& node) {
    auto PreCond = evalCond(node.getCond());

    LoopFixupsStack.push_back({std::move(PreCond.TruePaths), {}, std::move(PreCond.FalsePaths)});

    BasicBlock* BodyBB = &*std::prev(curFunc()->end());
    {
        VariablesScoped Scope(Vars);
        node.getBody()->accept(*this);
        if (ExprRes.has_value())
            ExprRes = std::nullopt;

        std::prev(LoopFixupsStack.end())->Condition.push_back(Ir.addInstruction(Opcodes::BR)
                                                                .addEmptyBrDst());
    }

    applyFixups(std::move(std::prev(LoopFixupsStack.end())->Condition));
    auto Cond = evalCond(node.getCond());

    applyFixups(std::move(std::prev(LoopFixupsStack.end())->Body), BodyBB);
    applyFixups(std::move(Cond.TruePaths), BodyBB);

    applyFixups(std::move(std::prev(LoopFixupsStack.end())->Fin));
    applyFixups(std::move(Cond.FalsePaths), BodyBB);

    assert(std::prev(LoopFixupsStack.end())->Body.size() == 0);
    assert(std::prev(LoopFixupsStack.end())->Condition.size() == 0);
    assert(std::prev(LoopFixupsStack.end())->Fin.size() == 0);

    LoopFixupsStack.pop_back();
}

void IRBuilder::visit(const BreakNode&) {
    std::prev(LoopFixupsStack.end())->Fin.push_back(Ir.addInstruction(Opcodes::BR).addEmptyBrDst());
}

void IRBuilder::visit(const ContinueNode&) {
    std::prev(LoopFixupsStack.end())->Condition.push_back(Ir.addInstruction(Opcodes::BR).addEmptyBrDst());
}

