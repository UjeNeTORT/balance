#include "AST/ConstantFolding.h"
#include "AST/Node.h"
#include "Utils/Utils.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <variant>
#include <optional>

using namespace Balance;
using namespace AST;

namespace {

float convertToFloat(std::variant<int, float> Value) {
    return std::visit(overloaded {
                [](float& Val) { return Val; },
                [](int& Val)   { return (float)Val; }
            }, Value);
}

template<typename T>
std::variant<int, float> evalConstBinary(BinaryOp Op, T Left, T Right) {
    switch (Op) {
        case BinaryOp::Add:          return Left +  Right;
        case BinaryOp::Sub:          return Left -  Right;
        case BinaryOp::Mul:          return Left *  Right;
        case BinaryOp::Less:         return Left <  Right;
        case BinaryOp::Greater:      return Left >  Right;
        case BinaryOp::LessEqual:    return Left <= Right;
        case BinaryOp::GreaterEqual: return Left >= Right;
        case BinaryOp::Equal:        return Left == Right;
        case BinaryOp::NotEqual:     return Left != Right;
        case BinaryOp::LogicalAnd:   return Left && Right;
        case BinaryOp::LogicalOr:    return Left || Right;

        case BinaryOp::Div:
            if (Right == 0)
                throw std::runtime_error("Division by zero");
            return Left / Right;

        case BinaryOp::Mod:
            if constexpr (std::is_same_v<T, float>) {
                throw std::runtime_error("Mod operation (%) isn't allowed for float operands");
            } else {
                if (Right == 0)
                    throw std::runtime_error("Division by zero");
                return Left % Right;
            }

        default: unreachable("Unhandled operation");
    }
}

template<typename T>
std::variant<int, float> evalConstUnary(UnaryOp Op, T Val) {
    switch (Op) {
        case UnaryOp::Plus:       return Val;
        case UnaryOp::Minus:      return -Val;
        case UnaryOp::LogicalNot: return !Val;
        default: unreachable("Unhandled operation");
    }
}

} // anonymous namespace

void AstConstantFolder::visit(const CompUnitNode& node) {
    std::vector<NodePtr> NewItems;

    for (const auto* Item: node.getItems())
        NewItems.push_back(visitChildAndConstruct<Node>(Item));

    NewTree.setCompUnit(NewTree.construct<CompUnitNode>(std::move(NewItems)));
}

void AstConstantFolder::visit(const VarDeclNode& node) {
    std::vector<VarDefPtr> NewDefs;

    for (const auto* Def: node.getDefs())
        NewDefs.push_back(visitChildAndConstruct<VarDefNode>(Def));

    returnChild(std::make_unique<VarDeclNode>(node.getType(), std::move(NewDefs), node.isConst()));
}

void AstConstantFolder::visit(const VarDefNode& node) {
    std::vector<ExprPtr> NewDims;

    for (const auto* Dim: node.getDims())
        NewDims.push_back(visitChildAndConstruct<ExpressionNode>(Dim));

    InitValPtr NewInitVal = nullptr;
    if (node.hasInit())
        NewInitVal = visitChildAndConstruct<InitValNode>(node.getInit());

    returnChild(std::make_unique<VarDefNode>(node.getName(), std::move(NewDims), NewInitVal));
}

void AstConstantFolder::visit(const InitValNode& node) {
    if (!node.isList()) {
        returnChild(std::make_unique<InitValNode>(visitChildAndConstruct<ExpressionNode>(node.getExpr())));
        return;
    }

    std::vector<InitValNode*> NewList;
    for (const auto* Elem: node.getList())
        NewList.push_back(visitChildAndConstruct<InitValNode>(Elem));

    returnChild(std::make_unique<InitValNode>(std::move(NewList)));
}

void AstConstantFolder::visit(const FuncDefNode& node) {
    std::vector<FuncParamPtr> NewParams;

    for (const auto* Param: node.getParams())
        NewParams.push_back(visitChildAndConstruct<FuncParamNode>(Param));

    returnChild(std::make_unique<FuncDefNode>(node.getReturnType(), node.getName(),
                                              std::move(NewParams),
                                              visitChildAndConstruct<BlockNode>(node.getBody())));
}

void AstConstantFolder::visit(const FuncParamNode& node) {
    if (!node.isArray()) {
        returnChild(std::make_unique<FuncParamNode>(node.getType(), node.getName()));
        return;
    }

    std::vector<ExprPtr> NewDims;

    for (const auto* Dim: node.getDims())
        NewDims.push_back(visitChildAndConstruct<ExpressionNode>(Dim));

    returnChild(std::make_unique<FuncParamNode>(node.getType(), node.getName(), std::move(NewDims)));
}

void AstConstantFolder::visit(const BlockNode& node) {
    std::vector<StmtPtr> NewItems;

    for (const auto* Item: node.getItems())
        NewItems.push_back(visitChildAndConstruct<StatementNode>(Item));

    returnChild(std::make_unique<BlockNode>(std::move(NewItems)));
}

void AstConstantFolder::visit(const AssignNode& node) {
    returnChild(std::make_unique<AssignNode>(visitChildAndConstruct<LValNode>(node.getDest()),
                                             visitChildAndConstruct<ExpressionNode>(node.getSrc())));
}

void AstConstantFolder::visit(const ExprStmtNode& node) {
    if (!node.hasExpr()) {
        returnChild(std::make_unique<ExprStmtNode>());
        return;
    }

    returnChild(std::make_unique<ExprStmtNode>(visitChildAndConstruct<ExpressionNode>(node.getExpr())));
}

void AstConstantFolder::visit(const IfNode& node) {
    returnChild(std::make_unique<IfNode>(visitChildAndConstruct<ExpressionNode>(node.getCond()),
                                         visitChildAndConstruct<StatementNode>(node.getThen()),
                                         node.hasElse()
                                            ? visitChildAndConstruct<StatementNode>(node.getElse())
                                            : nullptr));
}

void AstConstantFolder::visit(const WhileNode& node) {
    returnChild(std::make_unique<WhileNode>(visitChildAndConstruct<ExpressionNode>(node.getCond()),
                                            visitChildAndConstruct<StatementNode>(node.getBody())));
}

void AstConstantFolder::visit(const BreakNode&) {
    returnChild(std::make_unique<BreakNode>());
}

void AstConstantFolder::visit(const ContinueNode&) {
    returnChild(std::make_unique<ContinueNode>());
}

void AstConstantFolder::visit(const ReturnNode& node) {
    if (!node.hasExpr()) {
        returnChild(std::make_unique<ReturnNode>());
        return;
    }

    returnChild(std::make_unique<ReturnNode>(visitChildAndConstruct<ExpressionNode>(node.getExpr())));
}

void AstConstantFolder::visit(const BinaryOpNode& node) {
    auto NewLeft = visitChild<ExpressionNode>(node.getLeft());
    auto NewRight = visitChild<ExpressionNode>(node.getRight());

    auto ConstLeft = getConstVal(NewLeft.get());
    auto ConstRight = getConstVal(NewRight.get());

    if (ConstLeft.has_value() && ConstRight.has_value()) {
        std::variant<int, float> Result;

        if (std::holds_alternative<float>(*ConstLeft) || std::holds_alternative<float>(*ConstRight)) {
            Result = evalConstBinary(node.getOp(), convertToFloat(*ConstLeft),
                                                   convertToFloat(*ConstRight));
        } else {
            Result = evalConstBinary(node.getOp(), *std::get_if<int>(&*ConstLeft),
                                                   *std::get_if<int>(&*ConstRight));
        }

        std::visit(overloaded {
            [this](int& Val)   { returnChild(std::make_unique<IntLiteralNode>(Val)); },
            [this](float& Val) { returnChild(std::make_unique<FloatLiteralNode>(Val)); }
        }, Result);
        return;
    }

    returnChild(std::make_unique<BinaryOpNode>(NewTree.construct(std::move(NewLeft)),
                                               node.getOp(),
                                               NewTree.construct(std::move(NewRight))));
}

void AstConstantFolder::visit(const UnaryOpNode& node) {
    auto NewOperand = visitChild<ExpressionNode>(node.getOperand());

    auto ConstOperand = getConstVal(NewOperand.get());

    if (ConstOperand.has_value()) {
        auto Result = std::visit(overloaded {
            [node](int& Val)   { return evalConstUnary(node.getOp(), Val); },
            [node](float& Val) { return evalConstUnary(node.getOp(), Val); }
        }, *ConstOperand);

        std::visit(overloaded {
            [this](int& Val)   { returnChild(std::make_unique<IntLiteralNode>(Val)); },
            [this](float& Val) { returnChild(std::make_unique<FloatLiteralNode>(Val)); }
        }, Result);
        return;
    }

    returnChild(std::make_unique<UnaryOpNode>(NewTree.construct(std::move(NewOperand)), node.getOp()));
}

void AstConstantFolder::visit(const CallNode& node) {
    std::vector<ExprPtr> NewArgs;
    for (const auto* Arg: node.getArgs())
        NewArgs.push_back(visitChildAndConstruct<ExpressionNode>(Arg));

    returnChild(std::make_unique<CallNode>(node.getCallee(), std::move(NewArgs)));
}

void AstConstantFolder::visit(const LValNode& node) {
    std::vector<ExprPtr> NewIndices;
    for (const auto* Indice: node.getIndices())
        NewIndices.push_back(visitChildAndConstruct<ExpressionNode>(Indice));

    if (!Vars) {
        returnChild(std::make_unique<LValNode>(node.getName(), std::move(NewIndices)));
        return;
    }

    auto Var = Vars->findVar(node.getName());
    if (NewIndices.size() != Var.Type.getSize())
        throw std::runtime_error("Array access has invalid number of dimensions");

    size_t Offset = 0;
    for (size_t i = 0; i < NewIndices.size(); i++) {
        auto Val = getConstVal(NewIndices[i]);
        if (!Val)
            throw std::runtime_error("Global variable initialization must be constant expression");
        Offset += convertToFloat(*Val) * Var.Type.getSubdimSize(i + 1);
    }

    auto Val = std::visit([Var, Offset](auto& Vec) {
        assert(Vec.size() > 0);
        assert(Offset % Var.Type.getSize() == 0);
        if (Offset / Var.Type.getSize() >= Vec.size())
            throw std::runtime_error("Array access is out of bounds");
        return ImmBaseVariant(Vec[Offset / Var.Type.getSize()]);
    }, Var.getGData()->getInit());

    std::visit(overloaded {
        [this](int& Int)     { returnChild(std::make_unique<IntLiteralNode>(Int)); },
        [this](float& Float) { returnChild(std::make_unique<FloatLiteralNode>(Float)); }
    }, Val);
}

void AstConstantFolder::visit(const IntLiteralNode& node) {
    returnChild(std::make_unique<IntLiteralNode>(node.getValue()));
}

void AstConstantFolder::visit(const FloatLiteralNode& node) {
    returnChild(std::make_unique<FloatLiteralNode>(node.getValue()));
}

