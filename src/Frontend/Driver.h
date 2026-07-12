#ifndef FRONTEND_DRIVER_H
#define FRONTEND_DRIVER_H

#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "AST/Ast.h"
#include "AST/Node.h"
#include "parser.h"

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

    // Parses the given file, or stdin when the name is empty.
    // Returns 0 on success.
    int parse(const std::string& fileName)
    {
        File = fileName;

        Location.initialize(&File);

        scanBegin();

        yy::parser parser(*this);

        int status = parser();

        scanEnd();

        return status;
    }

  private:
    void scanBegin()
    {
        if (File.empty())
            yyin = stdin;
        else if (!(yyin = fopen(File.c_str(), "r")))
            throw std::runtime_error("Can't open input file: " + File);
    }

    void scanEnd()
    {
        if (yyin && yyin != stdin)
            fclose(yyin);
    }
};

} // namespace Balance

#endif // FRONTEND_DRIVER_H
