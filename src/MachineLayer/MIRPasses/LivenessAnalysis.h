#ifndef MIR_PASSES_LIVE_INS_OUTS_H
#define MIR_PASSES_LIVE_INS_OUTS_H

#include "MIRPass.h"

#include <string>

namespace Balance {

class MachineFunction;

class LivenessAnalysis final : public MIRPass {
public:
    LivenessAnalysis(const std::string &Name = "LivenessAnalysis") : MIRPass(Name) {}

    bool run(MachineFunction &MF) override;
};

} // namespace Balance


#endif // MIR_PASSES_LIVE_INS_OUTS_H
