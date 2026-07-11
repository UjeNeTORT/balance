#ifndef LIVE_RANGES_H
#define LIVE_RANGES_H

#include "MachineBB.h"
#include "MachineOperand.h"

#include <unordered_set>

namespace Balance {

std::unordered_set<Register> Defs(const MachineBB &MBB);
std::unordered_set<Register> Uses(const MachineBB &MBB);

class LiveRanges;

} // namespace Balance

#endif // LIVE_RANGES_H
