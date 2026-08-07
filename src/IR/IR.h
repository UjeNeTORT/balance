#ifndef IR_IR_H_
#define IR_IR_H_

#include "IR/BasicBlock.h"
#include "IR/Instruction.h"
#include "IR/Function.h"
#include "IR/Operand.h"

#include <array>
#include <cassert>
#include <list>
#include <map>
#include <optional>
#include <stdexcept>

namespace Balance {

class IR {
public:
    constexpr static std::string InternalFuncsPrefix = "0_internal_";

    IR(const IR&) = delete;
    IR& operator=(const IR&) = delete;

    IR(IR&&) = default;
    IR& operator=(IR&&) = default;

    explicit IR();

    using FunctionStorage = std::list<Function>;
    using iterator = FunctionStorage::iterator;
    using const_iterator = FunctionStorage::const_iterator;

    void verify() const {
        for (const auto& Func: Functions)
            Func.verify();
    }

    iterator addFunction(Function&& Func) {
        if (FunctionsMap.find(Func.getName()) != FunctionsMap.end())
            throw std::runtime_error("Duplicate function: " + std::string(Func.getName()));

        auto It = Functions.insert(Functions.end(), std::move(Func));
        FunctionsMap.emplace(It->getName(), It);
        return It;
    }

    iterator findFunction(std::string Name) {
        auto It = FunctionsMap.find(Name);
        if (It == FunctionsMap.end())
            return Functions.end();
        return It->second;
    }

    VirtRegister getNewVirtReg(OpType Type) {
        return getLastFunction()->getNewVirtReg(Type);
    }
    template<size_t NUM>
    std::array<VirtRegister, NUM> getNewVirtRegs(OpType Type) {
        return getLastFunction()->getNewVirtRegs<NUM>(Type);
    }

    iterator getLastFunction() {
        if (Functions.empty())
            return Functions.end();
        return std::prev(Functions.end());
    }

    Instruction& addInstruction(Opcodes Opcode,
                                        std::optional<SourceInfo> SrcInfo = std::nullopt) {
        assert(!Functions.empty());
        iterator LastFunc = std::prev(Functions.end());

        return LastFunc->addInstruction(Opcode, SrcInfo);
    }

    iterator       begin()        { return Functions.begin(); }
    iterator       end()          { return Functions.end(); }
    const_iterator cbegin() const { return Functions.cbegin(); }
    const_iterator cend()   const { return Functions.cend(); }
private:
    FunctionStorage Functions;

    std::map<std::string_view, iterator> FunctionsMap;
};

} // Balance

#endif // IR_IR_H_
