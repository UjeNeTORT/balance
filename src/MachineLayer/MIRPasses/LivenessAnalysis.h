#ifndef MIR_PASSES_LIVE_INS_OUTS_H
#define MIR_PASSES_LIVE_INS_OUTS_H

#include "Pass.h"

#include <string>

namespace Balance {

class MachineFunction;

class LivenessAnalysis final : public Pass {
public:
    LivenessAnalysis(const std::string &Name = "LivenessAnalysis") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
};

} // namespace Balance


#endif // MIR_PASSES_LIVE_INS_OUTS_H
