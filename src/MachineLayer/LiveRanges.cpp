#include "LiveRanges.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineOperand.h"

#include <algorithm>
#include <vector>

namespace Balance {

std::vector<Register> Defs(const MachineBB &MBB) {
    std::vector<Register> Defs;

    auto InsertIfIsNotSame = [&Defs](const Register &Reg) {
        if (std::find(Defs.begin(), Defs.end(), Reg) == Defs.end()) {
            Defs.push_back(Reg);
        }
    };

    for (const auto &MI : MBB) {
        const auto &MIDefs = MI.getDefs();
        std::for_each(MIDefs.begin(), MIDefs.end(), InsertIfIsNotSame);
    }

    return Defs;
}

std::vector<Register> Uses(const MachineBB &MBB) {
    std::vector<Register> Uses;
    auto InsertIfIsNotSame = [&Uses](const Register &Reg) {
        if (std::find(Uses.begin(), Uses.end(), Reg) == Uses.end()) {
            Uses.push_back(Reg);
        }
    };
    for (const auto &MI : MBB) {
        const auto &MIUses = MI.getUses();
        std::for_each(MIUses.begin(), MIUses.end(), InsertIfIsNotSame);
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
