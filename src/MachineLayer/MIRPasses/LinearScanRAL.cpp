#include "LinearScanRAL.h"
#include "MIROpcodes.h"
#include "MachineInst.h"
#include "RISCV/RISCVRegisters.h"
#include "RPOTraversal.h"
#include "Register.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <map>
#include <set>

using namespace Balance;

static unsigned getSpillSlotIdx(unsigned MIIdx) {
    return MIIdx + 1;
}

static unsigned getFillSlotIdx(unsigned MIIdx) {
    return MIIdx - 1;
}

void LinearScanRAL::updateRanges(const MachineBB *MBB, int LinBeginIdx) {
    const auto &LiveOuts = MBB->getLiveOuts();

    unsigned NInst = std::distance(MBB->begin(), MBB->end());

    unsigned RangeEndIdx = LinBeginIdx + NInst * LinearPeriod;
    auto InitializeLiveRange = [LinBeginIdx, RangeEndIdx, this] (const Register &Reg) {
        // by default every live out should get Live interval as wide as Parent MBB
        LiveIntervals.insert({Reg, LiveInterval(LinBeginIdx, RangeEndIdx, Reg)});
    };
    std::for_each(LiveOuts.begin(), LiveOuts.end(), InitializeLiveRange);

    for (int MIIdx = RangeEndIdx - LinearPeriod; MIIdx >= LinBeginIdx; MIIdx -= LinearPeriod) {
        const MachineInst *MI = LinearInstructions[MIIdx];
        assert(MI);
        for (const MachineOperand &MO : MI->getOperands()) {
            if (!MO.isReg()) continue;
            // TODO: we might want to include physical register in analysis as well
            if (MO.getReg().isPhysical()) continue;
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
}

void LinearScanRAL::linearizeInstructions(MachineFunction &MF) {
    std::list<MachineBB *> RPO = RPOTraversal(MF).getRPO();

    for (MachineBB *MBB : RPO) {
        for (MachineInst &MI : *MBB) {
            LinearInstructions.push_back(&MI);
            for (unsigned i = 0; i < LinearPeriod - 1; i++) {
                // spill, fill, extra
                LinearInstructions.push_back(nullptr);
            }
        }
    }

    unsigned LinearBeginIdx = LinearInstructions.size();
    for (auto MBBIt = RPO.rbegin(); MBBIt != RPO.rend(); ++MBBIt) {
        const MachineBB *MBB = *MBBIt;
        unsigned NInst = std::distance(MBB->begin(), MBB->end());
        LinearBeginIdx -= NInst * LinearPeriod;
        updateRanges(MBB, LinearBeginIdx);
    }

    #if 0
    // debug output
    std::for_each(LinearInstructions.begin(), LinearInstructions.end(), [](const MachineInst *MI) {
        if (MI == nullptr) return;
        std::cerr << "[lsra]: " << '(' << MI->getMBB()->getReferenceName() << ") " << *MI << '\n';
    });
    std::cerr << '\n';

    std::cerr << "after LiveIntervals construction \n";
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
    using namespace RISCV;

    linearizeInstructions(MF);

    // TODO: implement machine register info
    std::unordered_set<Register> Pool {
        RISCVRegister::T1, RISCVRegister::T2,
        RISCVRegister::T3, RISCVRegister::T4, RISCVRegister::T5, RISCVRegister::T6,
        RISCVRegister::A2, RISCVRegister::A3, RISCVRegister::A4,
        RISCVRegister::A5, RISCVRegister::A6, RISCVRegister::A7,
    };

    unsigned PoolSize = Pool.size();

    std::vector<LiveInterval *> SortedIntervals;
    for (auto &p : LiveIntervals) SortedIntervals.push_back(&p.second);
    std::sort(SortedIntervals.begin(), SortedIntervals.end(), [](const LiveInterval *a, const LiveInterval *b) {
        return a->StartIdx < b->StartIdx;
    });

    for (const auto &LI : SortedIntervals) {
        expireOldIntervals(*LI, Pool);

        if (LI->Reg.isPhysical()) {
            // TODO: if a register is physical, their LiveIntervals should
            // be first in sorted intervals list
            unreachable("Todo: remove from active pool");
            continue;
        }

        if (Active.size() == PoolSize) {
            spillAtInterval(*LI, Pool);
            continue;
        }

        Active.insert(LI);
        RegMapping[LI] = *Pool.begin();
        Pool.erase(Pool.begin());
    }

    applyRegMapping(MF);

    #if 1
    for (auto RM : RegMapping) {
        std::cerr << RM.first->Reg << " -> ";
        if (RM.second.isStack()) {
            std::cerr << "Stack[" << RM.second.getStackId() << "]\n";
        } else {
            std::cerr << RM.second.getReg() << '\n';
        }
    }
    #endif

    return false;
}

void LinearScanRAL::applyRegMapping(MachineFunction &MF) {
    for (unsigned MIIdx = 0; MIIdx < LinearInstructions.size(); MIIdx += LinearPeriod) {
        assert(LinearInstructions[MIIdx]);
        MachineInst &MI = *LinearInstructions[MIIdx];
        MachineBB *MBB = MI.getMBB();

        for (MachineOperand &MO : MI) {
            if (!MO.isReg()) continue;
            Register CurrReg = MO.getReg();
            if (CurrReg.isPhysical()) continue;
            assert(LiveIntervals.count(CurrReg) && "LiveIntervals must know about each virtual register");

            UniqueStorage &US = RegMapping[&LiveIntervals[CurrReg]];
            if (US.isReg()) {
                MO.setReg(US.getReg());
            } else if (US.isStack()) {
                // insert spill after MI
                auto &Spill = MBB->insertMI(MI.getIterator(), RISCVOpcode::SW)
                    .addReg(RISCV::RISCVRegister::SP)
                    .addImm(US.getStackId() * 4)
                    .addReg(RISCV::RISCVRegister::T0);
                LinearInstructions[getSpillSlotIdx(MIIdx)] = &Spill;

                // insert fill before MI
                if (MIIdx != 0) {
                    auto &Fill = MBB->insertMI(std::prev(MI.getIterator()), RISCVOpcode::LW)
                        .addReg(RISCV::RISCVRegister::T0)
                        .addReg(RISCV::RISCVRegister::SP)
                        .addImm(US.getStackId() * 4);

                    LinearInstructions[getFillSlotIdx(MIIdx)] = &Fill;
                }
                MO.setReg(RISCV::RISCVRegister::T0);
            } else {
                unreachable("what are we even doing here?");
            }
        }
    }
}

void LinearScanRAL::expireOldIntervals(const LiveInterval &LI, std::unordered_set<Register> &Pool) {
    for (auto It = Active.begin(); It != Active.end(); ++It) {
        if ((*It)->EndIdx >= LI.StartIdx) return;

        assert(RegMapping.at(*It).isReg());

        Pool.insert(RegMapping.at(*It).getReg());
        RegMapping.erase(*It);
        It = Active.erase(It);
    }
}

void LinearScanRAL::spillAtInterval(const LiveInterval &LI, std::unordered_set<Register> &Pool) {
    const LiveInterval &LastActiveLI = **Active.rbegin();
    if (LastActiveLI.EndIdx > LI.EndIdx) {
        RegMapping[&LI] = RegMapping[&LastActiveLI];
        RegMapping[&LastActiveLI] = getStackSlot();
        Active.erase(&LastActiveLI);
        Active.insert(&LI);
    } else {
        RegMapping[&LI] = getStackSlot();
    }
    return;
}

LinearScanRAL::UniqueStorage LinearScanRAL::getStackSlot() const {
    return UniqueStorage(StackSlotCnt++);
}

bool LinearScanRAL::LiveIntervalCmpIncEnd::operator() (const LiveInterval *lhs, const LiveInterval *rhs) const {
    assert(lhs && rhs && "Passed nullptr to comparison");
    return lhs->EndIdx < rhs->EndIdx;
}
