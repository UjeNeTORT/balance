#ifndef MIR_PASSES_VERIFIER_PASS_H
#define MIR_PASSES_VERIFIER_PASS_H

#include "MachineFunction.h"
#include "Pass.h"

namespace Balance {

class VerifierPass final : public Pass {
public:
    bool run(MachineFunction &MF) override;
};

} // namespace Balance


#endif // MIR_PASSES_VERIFIER_PASS_H
