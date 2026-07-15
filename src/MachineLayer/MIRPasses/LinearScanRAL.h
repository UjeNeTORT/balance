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

    struct UniqueStorage {
        enum Type { Stack, Register } Type;
        union {
            unsigned StackSlotId;
            class Register PhysReg;
        };

        UniqueStorage() : Type(Type::Stack), StackSlotId(0) {
            unreachable("default ctor for UniqueStorage");
        }
        UniqueStorage(class Register Reg) : Type(Type::Register), PhysReg{Reg} {}
        UniqueStorage(unsigned StackSlotId) : Type(Type::Stack), StackSlotId{StackSlotId} {}

        bool isStack() const { return Type == Type::Stack; }
        bool isReg() const { return Type == Type::Register; }
        unsigned getStackId() const { return StackSlotId; }
        class Register getReg() const { return PhysReg; }
    };

    struct LiveIntervalCmpIncEnd {
        bool operator() (const LiveInterval *lhs, const LiveInterval *rhs) const;
    };

    using LiveIntervalsSet = std::set<const LiveInterval *, LiveIntervalCmpIncEnd>;
    using RegMappingT = std::map<const LiveInterval *, UniqueStorage>;

    LiveIntervalsSet Active;
    std::vector<MachineInst *> LinearInstructions;
    std::map<Register, LiveInterval> LiveIntervals;
    RegMappingT RegMapping;

    const unsigned LinearPeriod = 4;
    mutable unsigned StackSlotCnt = 0;
public:
    LinearScanRAL(const std::string &Name = "LSRAL") : Pass(Name) {}

    bool run(MachineFunction &MF) override;
private:
    void updateRanges(const MachineBB *MBB, int LinBeginIdx);
    void linearizeInstructions(MachineFunction &MF);
    void expireOldIntervals(const LiveInterval &LI, LiveIntervalsSet &Active,
            RegMappingT &RegMapping, std::unordered_set<Register> &Pool) const;
    void spillAtInterval(const LiveInterval &LI, LiveIntervalsSet &Active,
            RegMappingT &RegMapping, std::unordered_set<Register> &Pool) const;
    UniqueStorage getStackSlot() const;
};

} // namespace Balance

#endif // MIR_PASSES_LINEAR_SCAN_RAL_H
