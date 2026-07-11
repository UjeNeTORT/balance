#ifndef MIR_PASSES_PASS_H
#define MIR_PASSES_PASS_H

#include "MachineFunction.h"

#include <string>

namespace Balance {

class Pass {

    std::string Name;
public:
    Pass(const std::string &Name) : Name(Name) {}

    virtual bool run(MachineFunction &MF) = 0;
    const std::string &getName() const { return Name; }

    virtual ~Pass() = default;
};

} // namespace Balance

#endif // MIR_PASSES_PASS_H
