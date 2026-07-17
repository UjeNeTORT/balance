#include "LinearScanRAL.h"
#include "MIROpcodes.h"
#include "MachineInst.h"
#include "RISCV/RISCVRegisters.h"
#include "RPOTraversal.h"
#include "Register.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

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

    #if 1
    dumpLiveIntervals();
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
        RegMapping.insert({LI, *Pool.begin()});
        Pool.erase(Pool.begin());
    }

    for (auto RM : RegMapping) {
        std::cerr << RM.first->Reg << *RM.first << " -> ";
        if (RM.second.isStack()) {
            std::cerr << "Stack[" << RM.second.getStackId() << "]\n";
        } else {
            std::cerr << RM.second.getReg() << '\n';
        }
    }

    applyRegMapping(MF);

    #if 0
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

            std::cerr << "LiveIntervals[CurrReg] = " << LiveIntervals[CurrReg] <<  "   [" << &LiveIntervals[CurrReg] << "]" << "\n";
            UniqueStorage &US = RegMapping.at(&LiveIntervals[CurrReg]);
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
    for (auto It = Active.begin(); It != Active.end(); ) {
        if ((*It)->EndIdx >= LI.StartIdx) {
            return;
        }

        assert(RegMapping.at(*It).isReg());

        Pool.insert(RegMapping.at(*It).getReg());
        It = Active.erase(It);
    }
}

void LinearScanRAL::spillAtInterval(const LiveInterval &LI, std::unordered_set<Register> &Pool) {
    const LiveInterval *lastActive = *Active.rbegin();
    if (lastActive->EndIdx > LI.EndIdx) {
        RegMapping.insert({&LI, RegMapping.at(lastActive)});
        RegMapping.at(lastActive) = getStackSlot();
        Active.erase(lastActive);
        Active.insert(&LI);
    } else {
        RegMapping.insert({&LI, getStackSlot()});
    }
}

LinearScanRAL::UniqueStorage LinearScanRAL::getStackSlot() const {
    return UniqueStorage(StackSlotCnt++);
}

bool LinearScanRAL::LiveIntervalCmpIncEnd::operator() (const LiveInterval *lhs, const LiveInterval *rhs) const {
    assert(lhs && rhs && "Passed nullptr to comparison");
    return lhs->EndIdx < rhs->EndIdx;
}

void LinearScanRAL::dumpLiveIntervals() const {
    std::vector<Register> vregs;
    for (const auto &p : LiveIntervals) {
        vregs.push_back(p.first);
    }
    std::sort(vregs.begin(), vregs.end(), [](const Register &a, const Register &b) {
        return a.getId() < b.getId();
    });

    std::vector<std::string> reg_names;
    for (const auto &reg : vregs) {
        std::stringstream ss;
        ss << reg;
        reg_names.push_back(ss.str());
    }

    size_t max_inst_width = 0;
    for (const auto *MI : LinearInstructions) {
        if (!MI) continue;
        std::stringstream ss;
        ss << "[lsra]: (" << MI->getMBB()->getReferenceName() << ") " << *MI;
        if (ss.str().length() > max_inst_width) {
            max_inst_width = ss.str().length();
        }
    }
    const size_t padding = 4;
    max_inst_width += padding;

    // Print header
    std::cerr << std::left << std::setw(max_inst_width) << "Instructions";
    for (const auto &name : reg_names) {
        std::cerr << " " << name;
    }
    std::cerr << "\n";

    // Print instructions and liveness
    for (unsigned i = 0; i < LinearInstructions.size(); i += LinearPeriod) {
        const MachineInst *MI = LinearInstructions[i];
        if (!MI) continue;

        std::stringstream ss;
        ss << "[lsra]: (" << MI->getMBB()->getReferenceName() << ") " << *MI;
        std::string inst_str = ss.str();
        std::cerr << std::left << std::setw(max_inst_width) << inst_str;

        for (size_t j = 0; j < vregs.size(); ++j) {
            const auto &li = LiveIntervals.at(vregs[j]);
            char liveness_char = (li.StartIdx <= i && i < li.EndIdx) ? '#' : '.';

            const std::string& name = reg_names[j];
            std::cerr << " " << std::string(name.length() / 2, ' ') << liveness_char << std::string(name.length() - name.length() / 2 - 1, ' ');
        }
        std::cerr << "\n";
    }
}
