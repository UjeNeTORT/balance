#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <vector>

#include "AST/Visitor.h"

namespace Balance
{

namespace AST
{

enum class BaseType
{
    Int,
    Float,
    Void,
};

enum class BinaryOp
{
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    Equal,
    NotEqual,
    LogicalAnd,
    LogicalOr,
};

enum class UnaryOp
{
    Plus,
    Minus,
    LogicalNot,
};

class Node
{
  public:
    virtual void accept(Visitor& visitor) const;

    virtual ~Node() = default;
};

class StatementNode : public Node
{
};

class ExpressionNode : public StatementNode
{
};

using NodePtr = Node*;
using StmtPtr = StatementNode*;
using ExprPtr = ExpressionNode*;

// ----- Expressions -----

class IntLiteralNode final : public ExpressionNode
{
  private:
    int Value;

  public:
    explicit IntLiteralNode(int value);

    int getValue() const { return Value; }

    void accept(Visitor& visitor) const override;
};

class FloatLiteralNode final : public ExpressionNode
{
  private:
    float Value;

  public:
    explicit FloatLiteralNode(float value);

    float getValue() const { return Value; }

    void accept(Visitor& visitor) const override;
};

class LValNode final : public ExpressionNode
{
  private:
    std::string Name;
    std::vector<ExprPtr> Indices;

  public:
    LValNode(std::string name, std::vector<ExprPtr>&& indices = {});

    const std::string& getName() const { return Name; }

    const std::vector<ExprPtr>& getIndices() const { return Indices; }

    void accept(Visitor& visitor) const override;
};

using LValPtr = LValNode*;

class BinaryOpNode final : public ExpressionNode
{
  private:
    ExprPtr Left;
    ExprPtr Right;
    BinaryOp Op;

  public:
    BinaryOpNode(ExprPtr left, BinaryOp op, ExprPtr right);

    BinaryOp getOp() const { return Op; }

    const ExpressionNode* getLeft() const { return Left; }

    const ExpressionNode* getRight() const { return Right; }

    void accept(Visitor& visitor) const override;
};

class UnaryOpNode final : public ExpressionNode
{
  private:
    ExprPtr Operand;
    UnaryOp Op;

  public:
    UnaryOpNode(ExprPtr operand, UnaryOp op);

    UnaryOp getOp() const { return Op; }

    const ExpressionNode* getOperand() const { return Operand; }

    void accept(Visitor& visitor) const override;
};

class CallNode final : public ExpressionNode
{
  private:
    std::string Callee;
    std::vector<ExprPtr> Args;

  public:
    CallNode(std::string callee, std::vector<ExprPtr>&& args = {});

    const std::string& getCallee() const { return Callee; }

    const std::vector<ExprPtr>& getArgs() const { return Args; }

    void accept(Visitor& visitor) const override;
};

// ----- Declarations -----

class InitValNode final : public Node
{
  private:
    ExprPtr Expr;
    std::vector<InitValNode*> List;
    bool IsList;

  public:
    explicit InitValNode(ExprPtr expr);

    explicit InitValNode(std::vector<InitValNode*>&& list);

    bool isList() const { return IsList; }

    const ExpressionNode* getExpr() const { return Expr; }

    const std::vector<InitValNode*>& getList() const { return List; }

    void accept(Visitor& visitor) const override;
};

using InitValPtr = InitValNode*;

class VarDefNode final : public Node
{
  private:
    std::string Name;
    std::vector<ExprPtr> Dims;
    InitValPtr Init;

  public:
    VarDefNode(std::string name, std::vector<ExprPtr>&& dims,
               InitValPtr init = nullptr);

    const std::string& getName() const { return Name; }

    const std::vector<ExprPtr>& getDims() const { return Dims; }

    bool hasInit() const { return Init != nullptr; }

    const InitValNode* getInit() const { return Init; }

    void accept(Visitor& visitor) const override;
};

using VarDefPtr = VarDefNode*;

class VarDeclNode final : public StatementNode
{
  private:
    BaseType Type;
    std::vector<VarDefPtr> Defs;
    bool IsConst;

  public:
    VarDeclNode(BaseType type, std::vector<VarDefPtr>&& defs, bool isConst);

    BaseType getType() const { return Type; }

    const std::vector<VarDefPtr>& getDefs() const { return Defs; }

    bool isConst() const { return IsConst; }

    void accept(Visitor& visitor) const override;
};

using VarDeclPtr = VarDeclNode*;

// ----- Functions -----

class FuncParamNode final : public Node
{
  private:
    BaseType Type;
    std::string Name;
    std::vector<ExprPtr> Dims;
    bool IsArray;

  public:
    FuncParamNode(BaseType type, std::string name);

    FuncParamNode(BaseType type, std::string name, std::vector<ExprPtr>&& dims);

    BaseType getType() const { return Type; }

    const std::string& getName() const { return Name; }

    const std::vector<ExprPtr>& getDims() const { return Dims; }

    bool isArray() const { return IsArray; }

    void accept(Visitor& visitor) const override;
};

using FuncParamPtr = FuncParamNode*;

class BlockNode final : public StatementNode
{
  private:
    std::vector<StmtPtr> Items;

  public:
    explicit BlockNode(std::vector<StmtPtr>&& items);

    const std::vector<StmtPtr>& getItems() const { return Items; }

    bool empty() const { return Items.empty(); }

    void accept(Visitor& visitor) const override;
};

using BlockPtr = BlockNode*;

class FuncDefNode final : public Node
{
  private:
    BaseType ReturnType;
    std::string Name;
    std::vector<FuncParamPtr> Params;
    BlockPtr Body;

  public:
    FuncDefNode(BaseType returnType, std::string name,
                std::vector<FuncParamPtr>&& params, BlockPtr body);

    BaseType getReturnType() const { return ReturnType; }

    const std::string& getName() const { return Name; }

    const std::vector<FuncParamPtr>& getParams() const { return Params; }

    const BlockNode* getBody() const { return Body; }

    void accept(Visitor& visitor) const override;
};

using FuncDefPtr = FuncDefNode*;

// ----- Statements -----

class AssignNode final : public StatementNode
{
  private:
    LValPtr Dest;
    ExprPtr Src;

  public:
    AssignNode(LValPtr dest, ExprPtr src);

    const LValNode* getDest() const { return Dest; }

    const ExpressionNode* getSrc() const { return Src; }

    void accept(Visitor& visitor) const override;
};

class ExprStmtNode final : public StatementNode
{
  private:
    ExprPtr Expr;

  public:
    explicit ExprStmtNode(ExprPtr expr = nullptr);

    bool hasExpr() const { return Expr != nullptr; }

    const ExpressionNode* getExpr() const { return Expr; }

    void accept(Visitor& visitor) const override;
};

class IfNode final : public StatementNode
{
  private:
    ExprPtr Cond;
    StmtPtr Then;
    StmtPtr Else;

  public:
    IfNode(ExprPtr cond, StmtPtr then, StmtPtr elseStmt = nullptr);

    const ExpressionNode* getCond() const { return Cond; }

    const StatementNode* getThen() const { return Then; }

    bool hasElse() const { return Else != nullptr; }

    const StatementNode* getElse() const { return Else; }

    void accept(Visitor& visitor) const override;
};

class WhileNode final : public StatementNode
{
  private:
    ExprPtr Cond;
    StmtPtr Body;

  public:
    WhileNode(ExprPtr cond, StmtPtr body);

    const ExpressionNode* getCond() const { return Cond; }

    const StatementNode* getBody() const { return Body; }

    void accept(Visitor& visitor) const override;
};

class BreakNode final : public StatementNode
{
  public:
    void accept(Visitor& visitor) const override;
};

class ContinueNode final : public StatementNode
{
  public:
    void accept(Visitor& visitor) const override;
};

class ReturnNode final : public StatementNode
{
  private:
    ExprPtr Expr;

  public:
    explicit ReturnNode(ExprPtr expr = nullptr);

    bool hasExpr() const { return Expr != nullptr; }

    const ExpressionNode* getExpr() const { return Expr; }

    void accept(Visitor& visitor) const override;
};

// ----- Compilation unit -----

class CompUnitNode final : public Node
{
  private:
    std::vector<NodePtr> Items;

  public:
    explicit CompUnitNode(std::vector<NodePtr>&& items);

    const std::vector<NodePtr>& getItems() const { return Items; }

    bool empty() const { return Items.empty(); }

    void accept(Visitor& visitor) const override;
};

using CompUnitPtr = CompUnitNode*;

} // namespace AST

} // namespace Balance

#endif // AST_NODE_H
