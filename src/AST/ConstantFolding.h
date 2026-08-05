#ifndef AST_CONSTANT_FOLDING_H_
#define AST_CONSTANT_FOLDING_H_

#include "AST/Ast.h"
#include "AST/Node.h"
#include "AST/Visitor.h"

namespace Balance {

namespace AST {

class AstConstantFolder final : public Visitor {
public:
    Ast fold(Ast&& OldAst) && {
        OldAst.getCompUnit()->accept(*this);
        return std::move(NewTree);
    }

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

private:
    Ast NewTree;

    std::unique_ptr<Node> RetChild = nullptr;

    template<typename T>
    std::unique_ptr<T> visitChild(const Node* Child) {
        assert(Child != nullptr);
        assert(RetChild == nullptr);
        Child->accept(*this);
        assert(RetChild);

        assert(dynamic_cast<T*>(RetChild.get())); // static_cast is optimizaion
        return std::unique_ptr<T>(static_cast<T*>(RetChild.release()));
    }

    template<typename T>
    T* visitChildAndConstruct(const Node* Child) {
        return NewTree.construct(visitChild<T>(Child));
    }

    void returnChild(std::unique_ptr<Node> Child) {
        assert(Child != nullptr);
        assert(RetChild == nullptr);
        RetChild = std::move(Child);
    }
};

} // namespace AST

} // namespace Balance

#endif // AST_CONSTANT_FOLDING_H_
