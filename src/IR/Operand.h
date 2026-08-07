#ifndef IR_OPERAND_H_
#define IR_OPERAND_H_

#include "Utils/Utils.h"

#include <cassert>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace Balance {

class BasicBlock;
class Instruction;

class OpBaseType {
public:
    static constexpr bool isArray() { return false; }
};

class OpInt: public OpBaseType {
public:
    static constexpr size_t getSize() { return 4; }
    static constexpr bool isInt() { return true; }
};
class OpFloat: public OpBaseType {
public:
    static constexpr size_t getSize() { return 4; }
    static constexpr bool isInt() { return false; }
};

template<typename T>
class OpArray: public std::vector<size_t> {
public:
    using array_t = T;

    using std::vector<size_t>::vector;

    OpArray(const std::vector<size_t>& Other)
        : std::vector<size_t>(Other)
    {}

    bool isArray() const { return size() != 0; }
    bool isInt()   const { return isArray() || T::isInt(); }

    size_t getSize() {
        size_t Size = T::getSize();
        for (auto Dim: *this)
            Size *= Dim;
        return Size;
    }

    bool operator==(const OpArray<T>& Other) const {
        if (size() != Other.size())
            return false;
        for (size_t i = 0; i < size(); i++) {
            if (i == 0 && (operator[](0) == 0 || Other[0] == 0)) // check for func arguments
                continue;
            if (operator[](i) != Other[i])
                return false;
        }
        return true;
    }
    bool operator!=(const OpArray<T>& Other) const {
        return !(*this == Other);
    }

    bool operator==(const T&) const { return !isArray(); }
    bool operator!=(const T& Other) const { return !(*this == Other); }
    friend bool operator==(const T& a, const OpArray<T>& b) { return b == a; }
    friend bool operator!=(const T& a, const OpArray<T>& b) { return b != a; }
};
using OpIntArray = OpArray<OpInt>;
using OpFloatArray = OpArray<OpFloat>;

using OpTypeVariant = std::variant<OpInt, OpFloat, OpIntArray, OpFloatArray>;

class OpType: public OpTypeVariant {
public:
    using OpTypeVariant::variant;

    bool isArray() const { return std::visit([](auto& Op) { return Op.isArray(); }, *this); }
    bool isInt()   const { return std::visit([](auto& Op) { return Op.isInt(); }, *this); }

    bool operator==(const OpType& OtherV) const {
        return std::visit([](const auto& This, const auto& Other){
            if constexpr (has_eq_v<decltype(This), decltype(Other)>) {
                return This == Other;
            } else if constexpr (std::is_same_v<decltype(This), decltype(Other)>) {
                static_assert(std::is_base_of_v<OpBaseType, std::decay_t<decltype(This)>>);
                return true;
            } else {
                return false;
            }
        }, *this, OtherV);
    }
    bool operator!=(const OpType& Other) const {
        return !(*this == Other);
    }
};

struct VirtRegister {
    OpType Type;
    int Id;

    std::optional<BasicBlock*> DefBlock;

    operator int() const { return Id; }
};

} // Balance

#endif // IR_OPERAND_H_
