#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include "MachineFunction.h"
#include "MIRPasses/MIRPass.h"
#include "Pass/PassManager.h"

namespace Balance {

using PassManager = FunctionPassManager<MachineFunction>;

} // namespace Balance

#endif // PASS_MANAGER_H
