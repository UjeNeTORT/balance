#ifndef AST_OP_NAMES_H
#define AST_OP_NAMES_H

#include <string_view>

#include "AST/Node.h"

namespace Balance
{

namespace AST
{

std::string_view typeName(BaseType type);

std::string_view opName(BinaryOp op);

std::string_view opName(UnaryOp op);

} // namespace AST

} // namespace Balance

#endif // AST_OP_NAMES_H
