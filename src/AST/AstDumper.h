#ifndef AST_AST_DUMPER_H
#define AST_AST_DUMPER_H

#include <ostream>

#include "AST/Node.h"
#include "AST/OpNames.h"
#include "AST/Visitor.h"

namespace Balance
{

namespace AST
{

// Prints the AST as an indented tree, one node per line.
class AstDumper final : public Visitor
{
  private:
    std::ostream& Out;
    int Depth = 0;

    std::ostream& line()
    {
        for (int i = 0; i < Depth; ++i)
            Out << "  ";

        return Out;
    }

    class Indent final
    {
      private:
        int& Depth;

      public:
        explicit Indent(int& depth)
            : Depth(depth)
        {
            ++Depth;
        }

        ~Indent() { --Depth; }
    };

  public:
    explicit AstDumper(std::ostream& out)
        : Out(out)
    {}

    void visit(const CompUnitNode& node) override
    {
        line() << "CompUnit\n";

        Indent indent(Depth);

        for (const auto* item : node.getItems())
            item->accept(*this);
    }

    void visit(const VarDeclNode& node) override
    {
        line() << (node.isConst() ? "ConstDecl " : "VarDecl ")
               << typeName(node.getType()) << "\n";

        Indent indent(Depth);

        for (const auto* def : node.getDefs())
            def->accept(*this);
    }

    void visit(const VarDefNode& node) override
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

    void visit(const InitValNode& node) override
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

    void visit(const FuncDefNode& node) override
    {
        line() << "FuncDef " << typeName(node.getReturnType()) << " "
               << node.getName() << "\n";

        Indent indent(Depth);

        for (const auto* param : node.getParams())
            param->accept(*this);

        node.getBody()->accept(*this);
    }

    void visit(const FuncParamNode& node) override
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

    void visit(const BlockNode& node) override
    {
        line() << "Block\n";

        Indent indent(Depth);

        for (const auto* item : node.getItems())
            item->accept(*this);
    }

    void visit(const AssignNode& node) override
    {
        line() << "Assign\n";

        Indent indent(Depth);

        node.getDest()->accept(*this);
        node.getSrc()->accept(*this);
    }

    void visit(const ExprStmtNode& node) override
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

    void visit(const IfNode& node) override
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

    void visit(const WhileNode& node) override
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

    void visit(const BreakNode&) override { line() << "Break\n"; }

    void visit(const ContinueNode&) override { line() << "Continue\n"; }

    void visit(const ReturnNode& node) override
    {
        line() << "Return\n";

        if (node.hasExpr())
        {
            Indent indent(Depth);
            node.getExpr()->accept(*this);
        }
    }

    void visit(const BinaryOpNode& node) override
    {
        line() << "BinaryOp " << opName(node.getOp()) << "\n";

        Indent indent(Depth);

        node.getLeft()->accept(*this);
        node.getRight()->accept(*this);
    }

    void visit(const UnaryOpNode& node) override
    {
        line() << "UnaryOp " << opName(node.getOp()) << "\n";

        Indent indent(Depth);
        node.getOperand()->accept(*this);
    }

    void visit(const CallNode& node) override
    {
        line() << "Call " << node.getCallee() << "\n";

        Indent indent(Depth);

        for (const auto* arg : node.getArgs())
            arg->accept(*this);
    }

    void visit(const LValNode& node) override
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

    void visit(const IntLiteralNode& node) override
    {
        line() << "IntLiteral " << node.getValue() << "\n";
    }

    void visit(const FloatLiteralNode& node) override
    {
        line() << "FloatLiteral " << node.getValue() << "\n";
    }
};

} // namespace AST

} // namespace Balance

#endif // AST_AST_DUMPER_H
