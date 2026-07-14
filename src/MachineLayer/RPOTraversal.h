#include "MachineFunction.h"
#include "MachineBB.h"

#include <list>

namespace Balance {
std::list<MachineBB *> getRPO(MachineFunction::iterator I);
} // namespace Balance
