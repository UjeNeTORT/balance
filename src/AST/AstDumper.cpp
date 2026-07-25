#include "AST/AstDumper.h"

namespace Balance
{

namespace AST
{

AstDumper::AstDumper(std::ostream& out)
    : Out(out)
{}

void AstDumper::visit(const CompUnitNode& node)
{
    line() << "CompUnit\n";

    Indent indent(Depth);

    for (const auto* item : node.getItems())
        item->accept(*this);
}

void AstDumper::visit(const VarDeclNode& node)
{
    line() << (node.isConst() ? "ConstDecl " : "VarDecl ")
           << typeName(node.getType()) << "\n";

    Indent indent(Depth);

    for (const auto* def : node.getDefs())
        def->accept(*this);
}

void AstDumper::visit(const VarDefNode& node)
{
    line() << "VarDef " << node.getName() << "\n";

    Indent indent(Depth);

    for (const auto* dim : node.getDims())
    {
        line() << "Dim\n";

        Indent dimIndent(Depth);
        dim->accept(*this);
    }

    if (node.hasInit())
        node.getInit()->accept(*this);
}

void AstDumper::visit(const InitValNode& node)
{
    if (!node.isList())
    {
        line() << "Init\n";

        Indent indent(Depth);
        node.getExpr()->accept(*this);

        return;
    }

    line() << "InitList\n";

    Indent indent(Depth);

    for (const auto* elem : node.getList())
        elem->accept(*this);
}

void AstDumper::visit(const FuncDefNode& node)
{
    line() << "FuncDef " << typeName(node.getReturnType()) << " "
           << node.getName() << "\n";

    Indent indent(Depth);

    for (const auto* param : node.getParams())
        param->accept(*this);

    node.getBody()->accept(*this);
}

void AstDumper::visit(const FuncParamNode& node)
{
    line() << "Param " << typeName(node.getType()) << " " << node.getName()
           << (node.isArray() ? "[]" : "") << "\n";

    Indent indent(Depth);

    for (const auto* dim : node.getDims())
    {
        line() << "Dim\n";

        Indent dimIndent(Depth);
        dim->accept(*this);
    }
}

void AstDumper::visit(const BlockNode& node)
{
    line() << "Block\n";

    Indent indent(Depth);

    for (const auto* item : node.getItems())
        item->accept(*this);
}

void AstDumper::visit(const AssignNode& node)
{
    line() << "Assign\n";

    Indent indent(Depth);

    node.getDest()->accept(*this);
    node.getSrc()->accept(*this);
}

void AstDumper::visit(const ExprStmtNode& node)
{
    if (!node.hasExpr())
    {
        line() << "EmptyStmt\n";
        return;
    }

    line() << "ExprStmt\n";

    Indent indent(Depth);
    node.getExpr()->accept(*this);
}

void AstDumper::visit(const IfNode& node)
{
    line() << "If\n";

    Indent indent(Depth);

    line() << "Cond\n";
    {
        Indent condIndent(Depth);
        node.getCond()->accept(*this);
    }

    line() << "Then\n";
    {
        Indent thenIndent(Depth);
        node.getThen()->accept(*this);
    }

    if (node.hasElse())
    {
        line() << "Else\n";

        Indent elseIndent(Depth);
        node.getElse()->accept(*this);
    }
}

void AstDumper::visit(const WhileNode& node)
{
    line() << "While\n";

    Indent indent(Depth);

    line() << "Cond\n";
    {
        Indent condIndent(Depth);
        node.getCond()->accept(*this);
    }

    line() << "Body\n";
    {
        Indent bodyIndent(Depth);
        node.getBody()->accept(*this);
    }
}

void AstDumper::visit(const BreakNode&) { line() << "Break\n"; }

void AstDumper::visit(const ContinueNode&) { line() << "Continue\n"; }

void AstDumper::visit(const ReturnNode& node)
{
    line() << "Return\n";

    if (node.hasExpr())
    {
        Indent indent(Depth);
        node.getExpr()->accept(*this);
    }
}

void AstDumper::visit(const BinaryOpNode& node)
{
    line() << "BinaryOp " << opName(node.getOp()) << "\n";

    Indent indent(Depth);

    node.getLeft()->accept(*this);
    node.getRight()->accept(*this);
}

void AstDumper::visit(const UnaryOpNode& node)
{
    line() << "UnaryOp " << opName(node.getOp()) << "\n";

    Indent indent(Depth);
    node.getOperand()->accept(*this);
}

void AstDumper::visit(const CallNode& node)
{
    line() << "Call " << node.getCallee() << "\n";

    Indent indent(Depth);

    for (const auto* arg : node.getArgs())
        arg->accept(*this);
}

void AstDumper::visit(const LValNode& node)
{
    line() << "LVal " << node.getName() << "\n";

    Indent indent(Depth);

    for (const auto* index : node.getIndices())
    {
        line() << "Index\n";

        Indent indexIndent(Depth);
        index->accept(*this);
    }
}

void AstDumper::visit(const IntLiteralNode& node)
{
    line() << "IntLiteral " << node.getValue() << "\n";
}

void AstDumper::visit(const FloatLiteralNode& node)
{
    line() << "FloatLiteral " << node.getValue() << "\n";
}

} // namespace AST

} // namespace Balance
