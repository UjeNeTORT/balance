#include "AST/AstDotDumper.h"

namespace Balance
{

namespace AST
{

AstDotDumper::AstDotDumper(std::ostream& out)
    : Out(out)
{}

void AstDotDumper::dump(const Node& root)
{
    Out << "digraph AST {\n"
        << "    node [shape=box, fontname=\"monospace\"];\n";

    root.accept(*this);

    Out << "}\n";
}

void AstDotDumper::visit(const CompUnitNode& node)
{
    int id = emitNode("CompUnit");

    for (const auto* item : node.getItems())
        visitChild(*item, id);
}

void AstDotDumper::visit(const VarDeclNode& node)
{
    int id = emitNode(std::string(node.isConst() ? "ConstDecl " : "VarDecl ")
                      + std::string(typeName(node.getType())));

    for (const auto* def : node.getDefs())
        visitChild(*def, id);
}

void AstDotDumper::visit(const VarDefNode& node)
{
    int id = emitNode("VarDef " + node.getName());

    for (const auto* dim : node.getDims())
    {
        visitChild(*dim, emitTagUnder("Dim", id));
    }

    if (node.hasInit())
        visitChild(*node.getInit(), id);
}

void AstDotDumper::visit(const InitValNode& node)
{
    if (!node.isList())
    {
        int id = emitTag("Init");
        visitChild(*node.getExpr(), id);

        return;
    }

    int id = emitTag("InitList");

    for (const auto* elem : node.getList())
        visitChild(*elem, id);
}

void AstDotDumper::visit(const FuncDefNode& node)
{
    int id = emitNode("FuncDef " + std::string(typeName(node.getReturnType()))
                      + " " + node.getName());

    for (const auto* param : node.getParams())
        visitChild(*param, id);

    visitChild(*node.getBody(), id);
}

void AstDotDumper::visit(const FuncParamNode& node)
{
    int id = emitNode("Param " + std::string(typeName(node.getType())) + " "
                      + node.getName() + (node.isArray() ? "[]" : ""));

    for (const auto* dim : node.getDims())
        visitChild(*dim, emitTagUnder("Dim", id));
}

void AstDotDumper::visit(const BlockNode& node)
{
    int id = emitNode("Block");

    for (const auto* item : node.getItems())
        visitChild(*item, id);
}

void AstDotDumper::visit(const AssignNode& node)
{
    int id = emitNode("Assign");

    visitChild(*node.getDest(), id);
    visitChild(*node.getSrc(), id);
}

void AstDotDumper::visit(const ExprStmtNode& node)
{
    if (!node.hasExpr())
    {
        emitNode("EmptyStmt");
        return;
    }

    int id = emitNode("ExprStmt");

    visitChild(*node.getExpr(), id);
}

void AstDotDumper::visit(const IfNode& node)
{
    int id = emitNode("If");

    visitChild(*node.getCond(), emitTagUnder("Cond", id));
    visitChild(*node.getThen(), emitTagUnder("Then", id));

    if (node.hasElse())
        visitChild(*node.getElse(), emitTagUnder("Else", id));
}

void AstDotDumper::visit(const WhileNode& node)
{
    int id = emitNode("While");

    visitChild(*node.getCond(), emitTagUnder("Cond", id));
    visitChild(*node.getBody(), emitTagUnder("Body", id));
}

void AstDotDumper::visit(const BreakNode&) { emitNode("Break"); }

void AstDotDumper::visit(const ContinueNode&) { emitNode("Continue"); }

void AstDotDumper::visit(const ReturnNode& node)
{
    int id = emitNode("Return");

    if (node.hasExpr())
        visitChild(*node.getExpr(), id);
}

void AstDotDumper::visit(const BinaryOpNode& node)
{
    int id = emitNode("BinaryOp " + escape(opName(node.getOp())));

    visitChild(*node.getLeft(), id);
    visitChild(*node.getRight(), id);
}

void AstDotDumper::visit(const UnaryOpNode& node)
{
    int id = emitNode("UnaryOp " + escape(opName(node.getOp())));

    visitChild(*node.getOperand(), id);
}

void AstDotDumper::visit(const CallNode& node)
{
    int id = emitNode("Call " + node.getCallee());

    for (const auto* arg : node.getArgs())
        visitChild(*arg, id);
}

void AstDotDumper::visit(const LValNode& node)
{
    int id = emitNode("LVal " + node.getName());

    for (const auto* index : node.getIndices())
        visitChild(*index, emitTagUnder("Index", id));
}

void AstDotDumper::visit(const IntLiteralNode& node)
{
    emitNode("IntLiteral " + std::to_string(node.getValue()));
}

void AstDotDumper::visit(const FloatLiteralNode& node)
{
    std::ostringstream value;
    value << node.getValue();

    emitNode("FloatLiteral " + value.str());
}

} // namespace AST

} // namespace Balance
