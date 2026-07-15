#include "LinearScanRAL.h"
#include "RPOTraversal.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <map>

using namespace Balance;

static unsigned getSpillSlotIdx(unsigned MIIdx) {
    return MIIdx + 1;
}

void LinearScanRAL::updateRanges(const MachineBB *MBB, int LinBeginIdx) {
    // const auto &LiveIns = MBB->getLiveIns();
    const auto &LiveOuts = MBB->getLiveOuts();

    unsigned NInst = std::distance(MBB->begin(), MBB->end());

    unsigned RangeEndIdx = LinBeginIdx + NInst * LinearPeriod;
    auto InitializeLiveRange = [LinBeginIdx, RangeEndIdx, this] (const Register &Reg) {
        // by default every live out should get Live interval as wide as Parent MBB
        LiveIntervals.insert({Reg, LiveInterval(LinBeginIdx, RangeEndIdx, Reg)});
    };
    std::for_each(LiveOuts.begin(), LiveOuts.end(), InitializeLiveRange);

    for (int MIIdx = RangeEndIdx - LinearPeriod; MIIdx > LinBeginIdx; MIIdx -= LinearPeriod) {
        const MachineInst *MI = LinearInstructions[MIIdx];
        assert(MI);
        for (const MachineOperand &MO : MI->getOperands()) {
            if (!MO.isReg()) continue;
            // TODO: we might want to include physical register in analysis as well
            if (MO.getReg().getType() == Register::Type::Physical) continue;
            if (MO.isUse()) {
                if (LiveIntervals.find(MO.getReg()) == LiveIntervals.end()) {
                    LiveIntervals.insert({MO.getReg(), LiveInterval(LinBeginIdx, MIIdx + LinearPeriod, MO.getReg())});
                } else {
                    LiveIntervals[MO.getReg()].addRange(LinBeginIdx, MIIdx + LinearPeriod);
                }
            }
            if (MO.isDef()) {
                if (LiveIntervals.find(MO.getReg()) == LiveIntervals.end()) {
                    LiveIntervals.insert({MO.getReg(), LiveInterval(MIIdx, MIIdx + LinearPeriod, MO.getReg())});
                } else {
                    LiveIntervals[MO.getReg()].StartIdx = MIIdx;
                }
            }
        }

    }

    #if 0
    // debug output
    std::cerr << "after LiveIntervals construction for " << MBB->getReferenceName() << "\n";
    std::for_each(LiveIntervals.begin(), LiveIntervals.end(), [&, this](const std::pair<Register, LiveInterval> &PI) {
        std::cerr << PI.first << ": ";
        for (unsigned i = 0; i < LinearInstructions.size(); i += LinearPeriod) {
            if (PI.second.StartIdx <= i && i < PI.second.EndIdx) {
                std::cerr << "#";
            } else {
                std::cerr << "-";
            }
        }
        std::cerr << "\n";
        });
    #endif
}

bool LinearScanRAL::run(MachineFunction &MF) {
    std::list<MachineBB *> RPO = RPOTraversal(MF).getRPO();

    for (const MachineBB *MBB : RPO) {
        std::for_each(MBB->begin(), MBB->end(), [this](const MachineInst &MI) {
            LinearInstructions.push_back(&MI);
            for (unsigned i = 0; i < LinearPeriod - 1; i++) {
                // spill, fill, extra
                LinearInstructions.push_back(nullptr);
            }
        });
    }

    #if 0
    std::for_each(LinearInstructions.begin(), LinearInstructions.end(), [](const MachineInst *MI) {
        if (MI == nullptr) return;
        std::cerr << "[lsra]: " << '(' << MI->getMBB()->getReferenceName() << ") " << *MI << '\n';
    });
    std::cerr << '\n';
    #endif

    unsigned LinearBeginIdx = LinearInstructions.size();

    for (auto MBBIt = RPO.rbegin(); MBBIt != RPO.rend(); ++MBBIt) {
        const MachineBB *MBB = *MBBIt;
        unsigned NInst = std::distance(MBB->begin(), MBB->end());
        LinearBeginIdx -= NInst * LinearPeriod;
        updateRanges(MBB, LinearBeginIdx);
    }

    return false;
}
