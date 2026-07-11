#include "LiveRanges.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineOperand.h"

#include <unordered_set>

namespace Balance {

std::unordered_set<const MachineOperand *> Defs(const MachineBB &MBB) {
    std::unordered_set<const MachineOperand *> Defs;
    for (const auto &MI : MBB) {
        for (const auto &MO : MI) {
            if (isVirtReg(MO) && isDef(MO)) {
                Defs.insert(&MO);
            }
        }
    }
    return Defs;
}

std::unordered_set<const MachineOperand *> Uses(const MachineBB &MBB) {
    std::unordered_set<const MachineOperand *> Uses;
    for (const auto &MI : MBB) {
        for (const auto &MO : MI) {
            if (isVirtReg(MO) && isUse(MO)) {
                Uses.insert(&MO);
            }
        }
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
