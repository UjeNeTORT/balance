#ifndef MIR_PASSES_DCE_H
#define MIR_PASSES_DCE_H

#include "Pass.h"

#include <string>

namespace Balance {

class MachineFunction;

class DeadCodeElimination : public Pass {
public:
    DeadCodeElimination(const std::string &Name = "dce") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
    void removeDef(MachineFunction &MF, Register Reg);
};

} // namespace Balance

#endif // MIR_PASSES_DCE_H
