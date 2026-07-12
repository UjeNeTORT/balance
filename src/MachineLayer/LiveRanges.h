#ifndef LIVE_RANGES_H
#define LIVE_RANGES_H

#include "MachineBB.h"
#include "MachineOperand.h"

#include <unordered_set>

namespace Balance {

std::unordered_set<Register> ComputeDefs(const MachineBB &MBB);
std::unordered_set<Register> ComputeUses(const MachineBB &MBB);
std::unordered_set<Register> ComputeUpwardExposed(const MachineBB &MBB);
std::unordered_set<Register> ComputePhiDefs(const MachineBB &MBB);
std::unordered_set<Register> ComputePhiUses(const MachineBB &MBB);
std::unordered_set<Register> ComputeLiveIns(const MachineBB &MBB);
std::unordered_set<Register> ComputeLiveOuts(const MachineBB &MBB);

class LiveRanges;

} // namespace Balance

#endif // LIVE_RANGES_H
