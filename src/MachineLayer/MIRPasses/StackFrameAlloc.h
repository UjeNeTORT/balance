#ifndef MIR_PASSES_STACK_FRAME_ALLOC_H
#define MIR_PASSES_STACK_FRAME_ALLOC_H

#include "Pass.h"

namespace Balance {

class StackFrameAlloc final : public Pass {
public:
    StackFrameAlloc(const std::string &Name = "StackFrameAlloc") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
};

} // namespace Balance

#endif // MIR_PASSES_STACK_FRAME_ALLOC_H
