#include "LivenessAnalysis.h"
#include "LiveRanges.h"

using namespace Balance;

bool LivenessAnalysis::run(MachineFunction &MF) {

    // reset
    for (auto &BB : MF) {
        BB.setLiveIns({});
        BB.setLiveOuts({});
    }

    bool Converged = false;
    while (!Converged) {
        Converged = true;
        for (auto &BB : MF) {
            auto OldLiveIns = BB.getLiveIns();
            auto OldLiveOuts = BB.getLiveOuts();

            auto NewLiveOuts = ComputeLiveOuts(BB);
            BB.setLiveOuts(NewLiveOuts);

            auto NewLiveIns = ComputeLiveIns(BB);
            BB.setLiveIns(NewLiveIns);

            if (OldLiveIns != NewLiveIns || OldLiveOuts != NewLiveOuts) {
                Converged = false;
            }
        }
    }

    // overall nothing has changed, despite BBs
    // now know their LiveIns and LiveOuts
    return false;
}
