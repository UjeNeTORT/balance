#ifndef AST_VISITOR_H
#define AST_VISITOR_H

namespace Balance
{

namespace AST
{

class CompUnitNode;
class VarDeclNode;
class VarDefNode;
class InitValNode;
class FuncDefNode;
class FuncParamNode;
class BlockNode;
class AssignNode;
class ExprStmtNode;
class IfNode;
class WhileNode;
class BreakNode;
class ContinueNode;
class ReturnNode;
class BinaryOpNode;
class UnaryOpNode;
class CallNode;
class LValNode;
class IntLiteralNode;
class FloatLiteralNode;

class Visitor
{
  public:
    virtual void visit(const CompUnitNode& node) = 0;
    virtual void visit(const VarDeclNode& node) = 0;
    virtual void visit(const VarDefNode& node) = 0;
    virtual void visit(const InitValNode& node) = 0;
    virtual void visit(const FuncDefNode& node) = 0;
    virtual void visit(const FuncParamNode& node) = 0;
    virtual void visit(const BlockNode& node) = 0;
    virtual void visit(const AssignNode& node) = 0;
    virtual void visit(const ExprStmtNode& node) = 0;
    virtual void visit(const IfNode& node) = 0;
    virtual void visit(const WhileNode& node) = 0;
    virtual void visit(const BreakNode& node) = 0;
    virtual void visit(const ContinueNode& node) = 0;
    virtual void visit(const ReturnNode& node) = 0;
    virtual void visit(const BinaryOpNode& node) = 0;
    virtual void visit(const UnaryOpNode& node) = 0;
    virtual void visit(const CallNode& node) = 0;
    virtual void visit(const LValNode& node) = 0;
    virtual void visit(const IntLiteralNode& node) = 0;
    virtual void visit(const FloatLiteralNode& node) = 0;

    virtual ~Visitor() = default;
};

} // namespace AST

} // namespace Balance

#endif // AST_VISITOR_H
