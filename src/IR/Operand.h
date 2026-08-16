#ifndef IR_OPERAND_H
#define IR_OPERAND_H

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
    static constexpr bool IsArray = false;
};

class OpInt: public OpBaseType {
public:
    static constexpr size_t getSize() { return 4; }
    static constexpr bool IsInt = true;
};
class OpFloat: public OpBaseType {
public:
    static constexpr size_t getSize() { return 4; }
    static constexpr bool IsInt = false;
};

template<typename T>
class OpArray: public std::vector<size_t> {
public:
    static constexpr bool IsArray = true;
    static constexpr bool IsInt = true; //< because it's address

    using array_t = T;

    using std::vector<size_t>::vector;

    OpArray(const std::vector<size_t>& Other)
        : std::vector<size_t>(Other)
    {}

    size_t getSubdimSize(size_t Depth) const {
        assert(Depth <= size());
        size_t Size = T::getSize();
        for (size_t i = Depth; i < size(); i++)
            Size *= operator[](i);
        return Size;
    }
    size_t getSize() const { return getSubdimSize(0); }

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

    bool operator==(const T&) const { return false; }
    bool operator!=(const T& Other) const { return !(*this == Other); }
    friend bool operator==(const T& a, const OpArray<T>& b) { return b == a; }
    friend bool operator!=(const T& a, const OpArray<T>& b) { return b != a; }
};

using OpTypeVariant = std::variant<OpInt, OpFloat, OpArray<OpInt>, OpArray<OpFloat>>;

class OpType: public OpTypeVariant {
public:
    using OpTypeVariant::variant;

    bool isArray() const { return std::visit([](auto& Op) {
                                    return std::decay_t<decltype(Op)>::IsArray; }, *this); }
    bool isInt()   const { return std::visit([](auto& Op) {
                                    return std::decay_t<decltype(Op)>::IsInt; }, *this); }

    size_t getSize() const { return std::visit([](auto& Op) { return Op.getSize(); }, *this); }

    size_t getSubdimSize(size_t Depth) const {
        return std::visit([Depth](const auto& Op) {
            if constexpr (std::decay_t<decltype(Op)>::IsArray) {
                return Op.getSubdimSize(Depth);
            } else {
                assert(0 && "for arrays only");
                return (size_t)0;
            }
        }, *this);
    }

    OpType getSubdimType(size_t Depth) const {
        return std::visit([Depth](auto& Op) {
            using T = std::decay_t<decltype(Op)>;
            if constexpr (T::IsArray) {
                assert(Depth <= Op.size());
                return OpType(T(Op.begin() + Depth, Op.end()));
            } else {
                assert(0 && "for arrays only");
                return OpType();
            }
        }, *this);
    }

    OpType makeArray(std::vector<size_t>&& Dims) const {
        return std::visit([Dims](const auto& Type) {
            using T = std::decay_t<decltype(Type)>;
            if constexpr (!T::IsArray) {
                return OpType(OpArray<T>(Dims));
            } else {
                assert(0 && "for plain types only");
                return OpType();
            }
        }, *this);
    }

    size_t getDepth() const {
        return std::visit([](const auto& Op) {
            using T = std::decay_t<decltype(Op)>;
            if constexpr (T::IsArray) {
                return Op.size();
            } else {
                return (size_t)0;
            }
        }, *this);
    }

    bool operator==(const OpType& OtherV) const {
        return std::visit([](const auto& This, const auto& Other){
            using ThisT = std::decay_t<decltype(This)>;
            using OtherT = std::decay_t<decltype(Other)>;
            if constexpr (has_eq_v<ThisT, OtherT>) {
                return This == Other;
            } else if constexpr (std::is_same_v<ThisT, OtherT>) {
                static_assert(std::is_base_of_v<OpBaseType, ThisT>);
                return true;
            } else {
                return false;
            }
        }, *this, OtherV);
    }
    bool operator!=(const OpType& Other) const {
        return !(*this == Other);
    }
    bool isArithmCompatible(const OpType& OtherV) const {
        return std::visit([](const auto& This, const auto& Other){
            using ThisT = std::decay_t<decltype(This)>;
            using OtherT = std::decay_t<decltype(Other)>;
            return ThisT::IsInt == OtherT::IsInt;
        }, *this, OtherV);
    }
};

struct VirtRegister {
    OpType Type;
    int Id;

    std::optional<BasicBlock*> DefBlock;

    operator int() const { return Id; }
};

using ImmBaseVariant = std::variant<int, float>;

using ImmBaseVectorVariant = MakeVectorVariant<ImmBaseVariant>::type;

class GlobalData {
public:
    GlobalData(std::string VarName, bool IsConst, ImmBaseVectorVariant&& InitVals) :
        Name(VarName), Const(IsConst), Init(InitVals) {}

    std::string_view getName() const { return Name; }
    bool isConst() const { return Const; }
    ImmBaseVectorVariant& getInit() { return Init; }
    void addInitVal(ImmBaseVariant Value) {
        std::visit([Value](auto& Vec) {
            using T = std::decay_t<typename std::decay_t<decltype(Vec)>::value_type>;
            Vec.push_back(std::visit([](auto& Val) { return (T)Val; }, Value));
        }, Init);
    }
    void setInitVals(ImmBaseVectorVariant&& Vals) { Init = Vals; }

private:
    std::string Name;
    bool Const;
    ImmBaseVectorVariant Init;
};

using ImmVariant = VariantAppend<ImmBaseVariant, GlobalData*>::type;

} // Balance

#endif // IR_OPERAND_H
