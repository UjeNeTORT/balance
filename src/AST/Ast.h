#ifndef AST_AST_H
#define AST_AST_H

#include <memory>
#include <utility>
#include <vector>

#include "AST/Node.h"

namespace Balance
{

namespace AST
{

// Owns every node; hands out raw non-owning pointers.
class Ast final
{
  private:
    std::vector<std::unique_ptr<Node>> Data;
    CompUnitPtr CompUnit = nullptr;

  public:
    template <typename NodeType, typename... Args>
    NodeType* construct(Args&&... args)
    {
        auto nodePtr = std::make_unique<NodeType>(std::forward<Args>(args)...);

        auto rawPtr = nodePtr.get();

        Data.push_back(std::move(nodePtr));

        return rawPtr;
    }

    void setCompUnit(CompUnitPtr compUnit) { CompUnit = compUnit; }

    const CompUnitNode* getCompUnit() const { return CompUnit; }
};

} // namespace AST

} // namespace Balance

#endif // AST_AST_H
