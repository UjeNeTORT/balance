#include "LiveRanges.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineOperand.h"

#include <algorithm>
#include <vector>
#include <unordered_set>

namespace Balance {

std::unordered_set<Register> Defs(const MachineBB &MBB) {
    std::unordered_set<Register> Defs;

    auto SetInsert = [&Defs](Register R) {
        Defs.insert(R);
    };

    for (const auto &MI : MBB) {
        const auto &MIDefs = MI.getDefs();
        std::for_each(MIDefs.begin(), MIDefs.end(), SetInsert);
    }

    return Defs;
}

std::unordered_set<Register> Uses(const MachineBB &MBB) {
    std::unordered_set<Register> Uses;

    auto SetInsert = [&Uses](Register R) {
        Uses.insert(R);
    };

    for (const auto &MI : MBB) {
        const auto &MIUses = MI.getUses();
        std::for_each(MIUses.begin(), MIUses.end(), SetInsert);
    }

    return Uses;
}

class LiveIns {
    MachineFunction *MF = nullptr;
public:
    LiveIns(MachineFunction *MF) : MF(MF) {}

    // void run()
};

} // namespace Balance
