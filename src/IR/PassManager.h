#ifndef IR_PASS_MANAGER_H
#define IR_PASS_MANAGER_H

#include "IRPasses/IRPass.h"
#include "Pass/PassManager.h"

namespace Balance {

using IRPassManager = FunctionPassManager<Function>;

} // namespace Balance

#endif // IR_PASS_MANAGER_H
