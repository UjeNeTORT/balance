#include "LinearScanRAL.h"
#include "RPOTraversal.h"

#include <algorithm>

using namespace Balance;

bool LinearScanRAL::run(MachineFunction &MF) {
    std::list<MachineBB *> RPO = RPOTraversal(MF).getRPO();

    std::vector<const MachineInst *> LinearInstructions;

    for (const MachineBB *MBB : RPO) {
        std::for_each(MBB->begin(), MBB->end(), [&LinearInstructions](const MachineInst &MI) {
            LinearInstructions.push_back(&MI);
            LinearInstructions.push_back(nullptr); // possible spill
            LinearInstructions.push_back(nullptr); // possible fill
            LinearInstructions.push_back(nullptr); // possible sp manipulation (maybe unused)
        });
    }

    std::for_each(LinearInstructions.begin(), LinearInstructions.end(), [](const MachineInst *MI) { std::cerr << *MI << ' '; });
    std::cerr << '\n';

    return false;
}

