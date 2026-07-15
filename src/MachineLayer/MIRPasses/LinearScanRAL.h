#ifndef MIR_PASSES_LINEAR_SCAN_RAL_H
#define MIR_PASSES_LINEAR_SCAN_RAL_H

#include "Pass.h"
#include "MachineFunction.h"

#include <algorithm>
#include <map>
#include <unordered_set>
#include <set>
#include <string>
#include <vector>
#include <limits>

namespace Balance {

class LinearScanRAL final : public Pass {
    struct LiveInterval {
        unsigned StartIdx = 0;
        unsigned EndIdx = std::numeric_limits<unsigned>::max();

        Register Reg;

        LiveInterval() = default;
        LiveInterval(unsigned S, unsigned E, Register R) : StartIdx(S), EndIdx(E), Reg(R) {}

        void addRange(unsigned S, unsigned E) {
            StartIdx = std::min(S, StartIdx);
            EndIdx = std::max(E, EndIdx);
        }

        bool operator<(const LiveInterval &Other) const {
            return EndIdx < Other.EndIdx;
        }

        bool operator>(const LiveInterval &Other) const {
            return Other < *this;
        }
    };


    std::vector<MachineInst *> LinearInstructions;
    std::map<Register, LiveInterval> LiveIntervals;

    const unsigned LinearPeriod = 4;
public:
    LinearScanRAL(const std::string &Name = "LSRAL") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
private:
    void updateRanges(const MachineBB *MBB, int LinBeginIdx);
    void ExpireOldIntervals(const LiveInterval &LI, std::set<LiveInterval *> &Active,
            std::map<LiveInterval *, Register> &RegMapping, std::unordered_set<Register> &Pool) const;
};

} // namespace Balance

#endif // MIR_PASSES_LINEAR_SCAN_RAL_H
