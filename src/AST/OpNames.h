#ifndef AST_OP_NAMES_H
#define AST_OP_NAMES_H

#include <string_view>

#include "AST/Node.h"

namespace Balance
{

namespace AST
{

inline std::string_view typeName(BaseType type)
{
    switch (type)
    {
        case BaseType::Int:
            return "int";
        case BaseType::Float:
            return "float";
        case BaseType::Void:
            return "void";
    }

    return "unknown";
}

inline std::string_view opName(BinaryOp op)
{
    switch (op)
    {
        case BinaryOp::Add:
            return "+";
        case BinaryOp::Sub:
            return "-";
        case BinaryOp::Mul:
            return "*";
        case BinaryOp::Div:
            return "/";
        case BinaryOp::Mod:
            return "%";
        case BinaryOp::Less:
            return "<";
        case BinaryOp::Greater:
            return ">";
        case BinaryOp::LessEqual:
            return "<=";
        case BinaryOp::GreaterEqual:
            return ">=";
        case BinaryOp::Equal:
            return "==";
        case BinaryOp::NotEqual:
            return "!=";
        case BinaryOp::LogicalAnd:
            return "&&";
        case BinaryOp::LogicalOr:
            return "||";
    }

    return "unknown";
}

inline std::string_view opName(UnaryOp op)
{
    switch (op)
    {
        case UnaryOp::Plus:
            return "+";
        case UnaryOp::Minus:
            return "-";
        case UnaryOp::LogicalNot:
            return "!";
    }

    return "unknown";
}

} // namespace AST

} // namespace Balance

#endif // AST_OP_NAMES_H
