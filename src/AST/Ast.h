#ifndef AST_AST_H
#define AST_AST_H

#include <cassert>
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
    Ast(const Ast&) = delete;
    Ast& operator=(const Ast&) = delete;

    Ast(Ast&&) = default;
    Ast& operator=(Ast&&) = default;

    explicit Ast() = default;

    template <typename NodeType, typename... Args>
    NodeType* construct(Args&&... args)
    {
        auto nodePtr = std::make_unique<NodeType>(std::forward<Args>(args)...);

        auto rawPtr = nodePtr.get();

        Data.push_back(std::move(nodePtr));

        return rawPtr;
    }

    template <typename NodeType>
    NodeType* construct(std::unique_ptr<NodeType> NodePtr)
    {
        auto rawPtr = NodePtr.get();

        Data.push_back(std::move(NodePtr));

        return rawPtr;
    }

    void setCompUnit(CompUnitPtr compUnit) {
        assert(CompUnit == nullptr);
        CompUnit = compUnit;
    }

    const CompUnitNode* getCompUnit() const { return CompUnit; }
};

} // namespace AST

} // namespace Balance

#endif // AST_AST_H
