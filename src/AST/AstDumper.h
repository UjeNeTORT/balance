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
    explicit AstDumper(std::ostream& out);

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

#endif // AST_AST_DUMPER_H
