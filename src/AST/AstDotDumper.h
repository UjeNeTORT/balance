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

class AstDotDumper final : public Visitor
{
  private:
    std::ostream& Out;
    int NextId = 0;
    int ParentId = -1;

    int emitNode(const std::string& label)
    {
        int id = NextId++;

        Out << "    n" << id << " [label=\"" << label << "\"];\n";

        if (ParentId >= 0)
            Out << "    n" << ParentId << " -> n" << id << ";\n";

        return id;
    }

    int emitTag(const std::string& label)
    {
        int id = NextId++;

        Out << "    n" << id << " [label=\"" << label
            << "\", color=gray, fontcolor=gray];\n";

        if (ParentId >= 0)
            Out << "    n" << ParentId << " -> n" << id << ";\n";

        return id;
    }

    void visitChild(const Node& child, int parentId)
    {
        int savedParent = ParentId;

        ParentId = parentId;
        child.accept(*this);

        ParentId = savedParent;
    }

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

  public:
    explicit AstDotDumper(std::ostream& out);

    void dump(const Node& root);

    void visit(const CompUnitNode& node) override;
    void visit(const VarDeclNode& node) override;
    void visit(const VarDefNode& node) override;
    void visit(const InitValNode& node) override;
    void visit(const FuncDefNode& node) override;
    void visit(const FuncParamNode& node) override;
    void visit(const BlockNode& node) override;
    void visit(const AssignNode& node) override;
    void visit(const ExprStmtNode& node) override;
    void visit(const IfNode& node) override;
    void visit(const WhileNode& node) override;
    void visit(const BreakNode&) override;
    void visit(const ContinueNode&) override;
    void visit(const ReturnNode& node) override;
    void visit(const BinaryOpNode& node) override;
    void visit(const UnaryOpNode& node) override;
    void visit(const CallNode& node) override;
    void visit(const LValNode& node) override;
    void visit(const IntLiteralNode& node) override;
    void visit(const FloatLiteralNode& node) override;
};

} // namespace AST

} // namespace Balance

#endif // AST_AST_DOT_DUMPER_H
