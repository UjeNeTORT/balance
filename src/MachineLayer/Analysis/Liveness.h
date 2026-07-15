#ifndef LIVE_RANGES_H
#define LIVE_RANGES_H

#include "MachineBB.h"
#include "MachineOperand.h"
#include "Register.h"

#include <vector>
#include <unordered_set>

namespace Balance {

class LiveRange {
    struct Segment {
        unsigned StartIdx;
        unsigned EndIdx;
    };

    Register Reg;
    std::vector<Segment> Segments;
public:
    Register getReg() const { return Reg; }
    void setReg(Register NewReg) { Reg = NewReg; }
    bool isAssignedPhysical() { return Reg.getType() == Register::Type::Physical; }
};

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
