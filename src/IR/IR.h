#ifndef IR_IR_H
#define IR_IR_H

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
#include <string>

namespace Balance {

class IR {
public:
    static std::string internalFunc(std::string Name) { return "internal_func_" + Name; }

    IR(const IR&) = delete;
    IR& operator=(const IR&) = delete;

    IR(IR&&) = default;
    IR& operator=(IR&&) = default;

    explicit IR() {}

    using FunctionStorage = std::list<Function>;
    using iterator = FunctionStorage::iterator;
    using const_iterator = FunctionStorage::const_iterator;

    using GlobalDataStorage = std::list<GlobalData>;
    using gdata_iterator = GlobalDataStorage::iterator;
    using gdata_const_iterator = GlobalDataStorage::const_iterator;

    void verify() const {
        for (const auto& Func: Functions)
            Func.verify();
    }

    Function* addFunction(std::string Name) {
        if (FunctionsMap.find(Name) != FunctionsMap.end())
            throw std::runtime_error("Duplicate function: " + Name);

        auto It = Functions.emplace(Functions.end(), std::move(Name));
        FunctionsMap.emplace(It->getName(), It);
        return &*It;
    }
    Function* addFunctionDecl(std::string Name) {
        if (FunctionsMap.find(Name) != FunctionsMap.end())
            throw std::runtime_error("Duplicate function: " + Name);

        auto It = Functions.emplace(Functions.end(), std::move(Name), true);
        FunctionsMap.emplace(It->getName(), It);
        return &*It;
    }
    Function* findFunction(std::string Name) {
        auto It = FunctionsMap.find(Name);
        if (It == FunctionsMap.end())
            return nullptr;
        return &*It->second;
    }

    GlobalData* addGlobalData(GlobalData&& Data) {
        if (GDataMap.find(Data.getName()) != GDataMap.end())
            throw std::runtime_error("Duplicate global variable: " + std::string(Data.getName()));

        auto It = GData.emplace(GData.end(), Data);
        GDataMap.emplace(It->getName(), It);
        return &*It;
    }
    GlobalData* findGlobalData(std::string Name) {
        auto It = GDataMap.find(Name);
        if (It == GDataMap.end())
            return nullptr;
        return &*It->second;
    }

    VirtRegister getNewVirtReg(OpType Type) {
        return getLastFunction()->getNewVirtReg(Type);
    }
    template<size_t NUM>
    std::array<VirtRegister, NUM> getNewVirtRegs(OpType Type) {
        return getLastFunction()->getNewVirtRegs<NUM>(Type);
    }

    Function* getLastFunction() {
        if (Functions.empty())
            return nullptr;
        return &*std::prev(Functions.end());
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

    gdata_const_iterator gdata_cbegin() const { return GData.cbegin(); }
    gdata_const_iterator gdata_cend()   const { return GData.cend(); }

private:
    FunctionStorage Functions;
    GlobalDataStorage GData;

    std::map<std::string_view, iterator> FunctionsMap;
    std::map<std::string_view, gdata_iterator> GDataMap;
};

} // Balance

#endif // IR_IR_H
