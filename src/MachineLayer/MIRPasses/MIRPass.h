#ifndef MIR_PASSES_MIRPASS_H
#define MIR_PASSES_MIRPASS_H

#include "MachineFunction.h"
#include "Pass/Pass.h"

namespace Balance {

class MIRPass : public Pass<MachineFunction> {
public:
    using Pass<MachineFunction>::Pass;
};

} // namespace Balance

#endif // MIR_PASSES_MIRPASS_H
