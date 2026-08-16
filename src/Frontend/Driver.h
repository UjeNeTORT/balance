#ifndef FRONTEND_DRIVER_H
#define FRONTEND_DRIVER_H

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "AST/Ast.h"
#include "AST/ConstantFolding.h"
#include "AST/IRBuilder.h"
#include "AST/Node.h"
#include "IR/IR.h"
#include "parser.hpp"

extern FILE* yyin;

#define YY_DECL yy::parser::symbol_type yylex(Balance::Driver& drv)

namespace Balance
{
class Driver;
}

YY_DECL;

namespace Balance
{

class Driver final
{
  private:
    yy::location Location;
    std::string File;
    AST::Ast Tree;

  public:
    yy::location& getLocation() { return Location; }

    template <typename NodeType, typename... Args>
    NodeType* construct(Args&&... args)
    {
        return Tree.construct<NodeType>(std::forward<Args>(args)...);
    }

    void formCompUnit(std::vector<AST::NodePtr>&& items)
    {
        Tree.setCompUnit(construct<AST::CompUnitNode>(std::move(items)));
    }

    const AST::CompUnitNode* getCompUnit() const { return Tree.getCompUnit(); }

    int parse(const std::string& fileName);

    void foldConstants() {
        Tree = AST::AstConstantFolder().fold(std::move(Tree));
    }

    IR buildIR() const {
        return AST::IRBuilder().build(Tree);
    }

  private:
    void scanBegin();
    void scanEnd();
};

} // namespace Balance

#endif // FRONTEND_DRIVER_H
