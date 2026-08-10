#ifndef MACHINE_LAYER_MIR_H_
#define MACHINE_LAYER_MIR_H_

#include "MachineFunction.h"
#include "MachineOperand.h"

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

    using MachineGDataStorage = std::list<MachineGData>;
    using gdata_iterator = MachineGDataStorage::iterator;
    using gdata_const_iterator = MachineGDataStorage::const_iterator;

    gdata_iterator       gdata_begin()       { return GData.begin(); }
    gdata_iterator       gdata_end()         { return GData.end(); }
    gdata_const_iterator gdata_begin() const { return GData.cbegin(); }
    gdata_const_iterator gdata_end()   const { return GData.cend(); }

    MachineFunction* addFunction(MachineFunction&& Func) {
        return &*Functions.insert(Functions.end(), Func);
    }
    MachineFunction* findFunction(std::string_view Name) {
        for (auto& Func: Functions)
            if (Func.getName() == Name)
                return &Func;
        return nullptr;
    }

    MachineGData* addGData(MachineGData&& Data) {
        return &*GData.insert(GData.end(), Data);
    }
    MachineGData* findGData(std::string_view Name) {
        for (auto& Data: GData)
            if (Data.getName() == Name)
                return &Data;
        return nullptr;
    }

private:
    FunctionStorage Functions;
    MachineGDataStorage GData;
};

} // Balance

#endif // MACHINE_LAYER_MIR_H_
