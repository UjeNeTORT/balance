#include "AST/Node.h"

#include <cassert>

namespace Balance
{

namespace AST
{

void Node::accept(Visitor&) const {
    assert(0 && "Node is abstract class");
}

IntLiteralNode::IntLiteralNode(int value)
    : Value(value)
{}

void IntLiteralNode::accept(Visitor& visitor) const { visitor.visit(*this); }

FloatLiteralNode::FloatLiteralNode(float value)
    : Value(value)
{}

void FloatLiteralNode::accept(Visitor& visitor) const
{
    visitor.visit(*this);
}

LValNode::LValNode(std::string name, std::vector<ExprPtr>&& indices)
    : Name(std::move(name))
    , Indices(std::move(indices))
{}

void LValNode::accept(Visitor& visitor) const { visitor.visit(*this); }

BinaryOpNode::BinaryOpNode(ExprPtr left, BinaryOp op, ExprPtr right)
    : Left(left)
    , Right(right)
    , Op(op)
{}

void BinaryOpNode::accept(Visitor& visitor) const { visitor.visit(*this); }

UnaryOpNode::UnaryOpNode(ExprPtr operand, UnaryOp op)
    : Operand(operand)
    , Op(op)
{}

void UnaryOpNode::accept(Visitor& visitor) const { visitor.visit(*this); }

CallNode::CallNode(std::string callee, std::vector<ExprPtr>&& args)
    : Callee(std::move(callee))
    , Args(std::move(args))
{}

void CallNode::accept(Visitor& visitor) const { visitor.visit(*this); }

InitValNode::InitValNode(ExprPtr expr)
    : Expr(expr)
    , IsList(false)
{}

InitValNode::InitValNode(std::vector<InitValNode*>&& list)
    : Expr(nullptr)
    , List(std::move(list))
    , IsList(true)
{}

void InitValNode::accept(Visitor& visitor) const { visitor.visit(*this); }

VarDefNode::VarDefNode(std::string name, std::vector<ExprPtr>&& dims,
                       InitValPtr init)
    : Name(std::move(name))
    , Dims(std::move(dims))
    , Init(init)
{}

void VarDefNode::accept(Visitor& visitor) const { visitor.visit(*this); }

VarDeclNode::VarDeclNode(BaseType type, std::vector<VarDefPtr>&& defs,
                         bool isConst)
    : Type(type)
    , Defs(std::move(defs))
    , IsConst(isConst)
{}

void VarDeclNode::accept(Visitor& visitor) const { visitor.visit(*this); }

FuncParamNode::FuncParamNode(BaseType type, std::string name)
    : Type(type)
    , Name(std::move(name))
    , IsArray(false)
{}

FuncParamNode::FuncParamNode(BaseType type, std::string name,
                             std::vector<ExprPtr>&& dims)
    : Type(type)
    , Name(std::move(name))
    , Dims(std::move(dims))
    , IsArray(true)
{}

void FuncParamNode::accept(Visitor& visitor) const { visitor.visit(*this); }

BlockNode::BlockNode(std::vector<StmtPtr>&& items)
    : Items(std::move(items))
{}

void BlockNode::accept(Visitor& visitor) const { visitor.visit(*this); }

FuncDefNode::FuncDefNode(BaseType returnType, std::string name,
                         std::vector<FuncParamPtr>&& params, BlockPtr body)
    : ReturnType(returnType)
    , Name(std::move(name))
    , Params(std::move(params))
    , Body(body)
{}

void FuncDefNode::accept(Visitor& visitor) const { visitor.visit(*this); }

AssignNode::AssignNode(LValPtr dest, ExprPtr src)
    : Dest(dest)
    , Src(src)
{}

void AssignNode::accept(Visitor& visitor) const { visitor.visit(*this); }

ExprStmtNode::ExprStmtNode(ExprPtr expr)
    : Expr(expr)
{}

void ExprStmtNode::accept(Visitor& visitor) const { visitor.visit(*this); }

IfNode::IfNode(ExprPtr cond, StmtPtr then, StmtPtr elseStmt)
    : Cond(cond)
    , Then(then)
    , Else(elseStmt)
{}

void IfNode::accept(Visitor& visitor) const { visitor.visit(*this); }

WhileNode::WhileNode(ExprPtr cond, StmtPtr body)
    : Cond(cond)
    , Body(body)
{}

void WhileNode::accept(Visitor& visitor) const { visitor.visit(*this); }

void BreakNode::accept(Visitor& visitor) const { visitor.visit(*this); }

void ContinueNode::accept(Visitor& visitor) const { visitor.visit(*this); }

ReturnNode::ReturnNode(ExprPtr expr)
    : Expr(expr)
{}

void ReturnNode::accept(Visitor& visitor) const { visitor.visit(*this); }

CompUnitNode::CompUnitNode(std::vector<NodePtr>&& items)
    : Items(std::move(items))
{}

void CompUnitNode::accept(Visitor& visitor) const { visitor.visit(*this); }

} // namespace AST

} // namespace Balance
