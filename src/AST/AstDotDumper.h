#ifndef AST_AST_DOT_DUMPER_H
#define AST_AST_DOT_DUMPER_H

#include <ostream>
#include <sstream>
#include <string>

#include "AST/Node.h"
#include "AST/OpNames.h"
#include "AST/Visitor.h"

namespace Balance
{

namespace AST
{

// Prints the AST as a Graphviz DOT graph.
// Render with: dot -Tsvg out.dot -o out.svg
class AstDotDumper final : public Visitor
{
  private:
    std::ostream& Out;
    int NextId = 0;
    int ParentId = -1;

    // Emits a node under the current parent and returns its id.
    int emitNode(const std::string& label)
    {
        int id = NextId++;

        Out << "    n" << id << " [label=\"" << label << "\"];\n";

        if (ParentId >= 0)
            Out << "    n" << ParentId << " -> n" << id << ";\n";

        return id;
    }

    // Emits a grey helper node (Cond, Dim, Index, ...) under the parent.
    int emitTag(const std::string& label)
    {
        int id = NextId++;

        Out << "    n" << id << " [label=\"" << label
            << "\", color=gray, fontcolor=gray];\n";

        if (ParentId >= 0)
            Out << "    n" << ParentId << " -> n" << id << ";\n";

        return id;
    }

    // Visits a child with the parent id set for the duration of the call.
    void visitChild(const Node& child, int parentId)
    {
        int savedParent = ParentId;

        ParentId = parentId;
        child.accept(*this);

        ParentId = savedParent;
    }

  public:
    explicit AstDotDumper(std::ostream& out)
        : Out(out)
    {}

    // Call on the root node instead of accept() to get graph delimiters.
    void dump(const Node& root)
    {
        Out << "digraph AST {\n"
            << "    node [shape=box, fontname=\"monospace\"];\n";

        root.accept(*this);

        Out << "}\n";
    }

    void visit(const CompUnitNode& node) override
    {
        int id = emitNode("CompUnit");

        for (const auto* item : node.getItems())
            visitChild(*item, id);
    }

    void visit(const VarDeclNode& node) override
    {
        int id = emitNode(std::string(node.isConst() ? "ConstDecl " : "VarDecl ")
                          + std::string(typeName(node.getType())));

        for (const auto* def : node.getDefs())
            visitChild(*def, id);
    }

    void visit(const VarDefNode& node) override
    {
        int id = emitNode("VarDef " + node.getName());

        for (const auto* dim : node.getDims())
        {
            visitChild(*dim, emitTagUnder("Dim", id));
        }

        if (node.hasInit())
            visitChild(*node.getInit(), id);
    }

    void visit(const InitValNode& node) override
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

    void visit(const FuncDefNode& node) override
    {
        int id = emitNode("FuncDef " + std::string(typeName(node.getReturnType()))
                          + " " + node.getName());

        for (const auto* param : node.getParams())
            visitChild(*param, id);

        visitChild(*node.getBody(), id);
    }

    void visit(const FuncParamNode& node) override
    {
        int id = emitNode("Param " + std::string(typeName(node.getType())) + " "
                          + node.getName() + (node.isArray() ? "[]" : ""));

        for (const auto* dim : node.getDims())
            visitChild(*dim, emitTagUnder("Dim", id));
    }

    void visit(const BlockNode& node) override
    {
        int id = emitNode("Block");

        for (const auto* item : node.getItems())
            visitChild(*item, id);
    }

    void visit(const AssignNode& node) override
    {
        int id = emitNode("Assign");

        visitChild(*node.getDest(), id);
        visitChild(*node.getSrc(), id);
    }

    void visit(const ExprStmtNode& node) override
    {
        if (!node.hasExpr())
        {
            emitNode("EmptyStmt");
            return;
        }

        int id = emitNode("ExprStmt");

        visitChild(*node.getExpr(), id);
    }

    void visit(const IfNode& node) override
    {
        int id = emitNode("If");

        visitChild(*node.getCond(), emitTagUnder("Cond", id));
        visitChild(*node.getThen(), emitTagUnder("Then", id));

        if (node.hasElse())
            visitChild(*node.getElse(), emitTagUnder("Else", id));
    }

    void visit(const WhileNode& node) override
    {
        int id = emitNode("While");

        visitChild(*node.getCond(), emitTagUnder("Cond", id));
        visitChild(*node.getBody(), emitTagUnder("Body", id));
    }

    void visit(const BreakNode&) override { emitNode("Break"); }

    void visit(const ContinueNode&) override { emitNode("Continue"); }

    void visit(const ReturnNode& node) override
    {
        int id = emitNode("Return");

        if (node.hasExpr())
            visitChild(*node.getExpr(), id);
    }

    void visit(const BinaryOpNode& node) override
    {
        int id = emitNode("BinaryOp " + escape(opName(node.getOp())));

        visitChild(*node.getLeft(), id);
        visitChild(*node.getRight(), id);
    }

    void visit(const UnaryOpNode& node) override
    {
        int id = emitNode("UnaryOp " + escape(opName(node.getOp())));

        visitChild(*node.getOperand(), id);
    }

    void visit(const CallNode& node) override
    {
        int id = emitNode("Call " + node.getCallee());

        for (const auto* arg : node.getArgs())
            visitChild(*arg, id);
    }

    void visit(const LValNode& node) override
    {
        int id = emitNode("LVal " + node.getName());

        for (const auto* index : node.getIndices())
            visitChild(*index, emitTagUnder("Index", id));
    }

    void visit(const IntLiteralNode& node) override
    {
        emitNode("IntLiteral " + std::to_string(node.getValue()));
    }

    void visit(const FloatLiteralNode& node) override
    {
        std::ostringstream value;
        value << node.getValue();

        emitNode("FloatLiteral " + value.str());
    }

  private:
    // Emits a tag node under an explicit parent and returns the tag id.
    int emitTagUnder(const std::string& label, int parentId)
    {
        int savedParent = ParentId;

        ParentId = parentId;
        int id = emitTag(label);

        ParentId = savedParent;

        return id;
    }

    static std::string escape(std::string_view text)
    {
        std::string result;

        for (char symbol : text)
        {
            if (symbol == '"' || symbol == '\\')
                result += '\\';

            result += symbol;
        }

        return result;
    }
};

} // namespace AST

} // namespace Balance

#endif // AST_AST_DOT_DUMPER_H
