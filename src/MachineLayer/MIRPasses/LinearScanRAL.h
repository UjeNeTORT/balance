#ifndef MIR_PASSES_LINEAR_SCAN_RAL_H
#define MIR_PASSES_LINEAR_SCAN_RAL_H

#include "Pass.h"
#include "MachineFunction.h"

#include <string>

namespace Balance {

class LinearScanRAL final : public Pass {
public:
    LinearScanRAL(const std::string &Name = "LSRAL") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
};

} // namespace Balance

#endif // MIR_PASSES_LINEAR_SCAN_RAL_H
