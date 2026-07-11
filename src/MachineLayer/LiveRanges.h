#ifndef LIVE_RANGES_H
#define LIVE_RANGES_H

#include "MachineBB.h"
#include "MachineOperand.h"

#include <vector>

namespace Balance {

std::vector<Register> Defs(const MachineBB &MBB);
std::vector<Register> Uses(const MachineBB &MBB);

class LiveRanges;

} // namespace Balance

#endif // LIVE_RANGES_H
