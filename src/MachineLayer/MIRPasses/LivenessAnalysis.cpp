#include "LivenessAnalysis.h"
#include "LiveRanges.h"

#include <algorithm>

using namespace Balance;

bool LivenessAnalysis::run(MachineFunction &MF) {
    bool Changed = false;

    MachineBB &Entry = *std::find_if(MF.begin(), MF.end(), [](MachineBB &B) -> bool {
        return B.getPredecessors().empty();
    });

    ComputeLiveIns(Entry);
    ComputeLiveOuts(Entry);

    return Changed;
}
