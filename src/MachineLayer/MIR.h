#ifndef MACHINE_LAYER_MIR_H_
#define MACHINE_LAYER_MIR_H_

#include "MachineFunction.h"

#include <list>

namespace Balance {

class MIR {
public:
    using FunctionStorage = std::list<MachineFunction>;
    using iterator = FunctionStorage::iterator;
    using const_iterator = FunctionStorage::const_iterator;

    iterator       begin()       { return Functions.begin(); }
    iterator       end()         { return Functions.end(); }
    const_iterator begin() const { return Functions.cbegin(); }
    const_iterator end()   const { return Functions.cend(); }

    MachineFunction* addFunction(MachineFunction&& Func) {
        return &*Functions.insert(Functions.end(), std::move(Func));
    }

    MachineFunction* findFunction(std::string_view Name) {
        for (auto& Func: Functions)
            if (Func.getName() == Name)
                return &Func;
        return nullptr;
    }

private:
    FunctionStorage Functions;
};

} // Balance

#endif // MACHINE_LAYER_MIR_H_
