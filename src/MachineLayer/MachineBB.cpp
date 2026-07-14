#include "LiveRanges.h"
#include "MachineBB.h"
#include "MachineFunction.h"
#include "MachineInst.h"
#include "Register.h"

#include <algorithm>
#include <cassert>
#include <unordered_set>

namespace Balance {

MachineBB::MachineBB(MachineFunction *MF, std::string Name) : MF(MF), LabelIdx(MF->getNewMBBIdx()), Name(Name) {
    updateReferenceName();
}


MachineInst &MachineBB::createMI(MachineBB::iterator I, RISCVOpcode Opcode) {
    MachineInst *MI = &*Instructions.emplace(I, Opcode);
    MI->setMBB(this);
    return *MI;
};

MachineInst &MachineBB::createMI(RISCVOpcode Opcode) {
    return createMI(end(), Opcode);
};

MachineInst &MachineBB::insertMI(MachineBB::iterator I, MachineInst MI) {
    MI.setMBB(this);
    I = Instructions.insert(I, MI);
    return *I;
};

MachineInst &MachineBB::insertMI(MachineInst MI) {
    return insertMI(end(), MI);
};

MachineBB::iterator MachineBB::eraseMI(MachineBB::iterator I) {
    return Instructions.erase(I);
}

int MachineBB::getLabelIdx() const {
    return LabelIdx;
}

void MachineBB::setLabelIdx(int LabelIdxNew) {
    LabelIdx = LabelIdxNew;
    ReferenceName = getReferenceName();
}

const std::unordered_set<Register> &MachineBB::getLiveIns() const {
    return LiveIns;
}

const std::unordered_set<Register> &MachineBB::getLiveOuts() const {
    return LiveOuts;
}

void MachineBB::setLiveIns(const std::unordered_set<Register> &LiveInsNew) {
    LiveIns = LiveInsNew;
}

void MachineBB::setLiveOuts(const std::unordered_set<Register> &LiveOutsNew) {
    LiveOuts = LiveOutsNew;
}

void MachineBB::addSuccessor(MachineBB *Succ) {
    assert(Succ);
    if (this == Succ) return;

    addSuccessorOneWay(Succ);
    Succ->addPredecessorOneWay(this);
}

void MachineBB::addPredecessor(MachineBB *Pred) {
    assert(Pred);
    if (this == Pred) return;

    Predecessors.push_back(Pred);
    addPredecessorOneWay(Pred);
    Pred->addSuccessorOneWay(this);
}

void MachineBB::addSuccessorOneWay(MachineBB *Succ) {
    assert(Succ);
    if (this == Succ) return;
    Successors.push_back(Succ);
}

void MachineBB::addPredecessorOneWay(MachineBB *Pred) {
    assert(Pred);
    if (this == Pred) return;
    Predecessors.push_back(Pred);
}

void MachineBB::removeSuccessor(MachineBB *Succ) {
    if (this == Succ) return;
    removeSuccessorOneWay(Succ);
    Succ->removePredecessorOneWay(this);
}

void MachineBB::removePredecessor(MachineBB *Pred) {
    if (this == Pred) return;
    removePredecessorOneWay(Pred);
    Pred->removeSuccessorOneWay(this);
}

void MachineBB::removeSuccessorOneWay(MachineBB *Succ) {
    if (this == Succ) return;
    Successors.remove(Succ);
}

void MachineBB::removePredecessorOneWay(MachineBB *Pred) {
    if (this == Pred) return;
    Predecessors.remove(Pred);
}

bool MachineBB::verify() const {
    bool Valid = true;
    std::for_each(succ_begin(), succ_end(), [&Valid, this](MachineBB *Succ) {
        Valid &= std::find(Succ->pred_begin(), Succ->pred_end(), this) != Succ->pred_end();
        // assert(Valid && "Successor does not have this MBB as a predecessor");
    });

    std::for_each(pred_begin(), pred_end(), [&Valid, this](MachineBB *Pred) {
        Valid &= std::find(Pred->succ_begin(), Pred->succ_end(), this) != Pred->succ_end();
        // assert(Valid && "Predecessor does not have this MBB as a successor")
    });

    // check that each MI stored in this MBB knows its MBB
    Valid &= std::all_of(Instructions.begin(), Instructions.end(), [this](const MachineInst &MI) {
        return MI.getMBB() == this;
    });

    return Valid;
}

MachineFunction *MachineBB::getMF() const { return MF; }
void MachineBB::setMF(MachineFunction *NewMF) { MF = NewMF; }


void MachineBB::updateReferenceName() {
    ReferenceName = std::string("MBB.") + std::to_string(LabelIdx);
}

std::string_view MachineBB::getReferenceName() const {
    return ReferenceName;
}

void MachineBB::print(std::ostream &OS) const {
    if (!Name.empty()) OS << "\"" << Name << "\"\n";

    auto &&DefsVec = ComputeDefs(*this);
    auto &&UsesVec = ComputeUses(*this);

    bool First = true;
    auto PrintNext = [&OS, &First](const auto &Reg) {
        if (First) First = false;
        else OS << ", ";
        OS << Reg;
    };

    OS << "Defs: ";
    std::for_each(DefsVec.begin(), DefsVec.end(), PrintNext);
    OS << "\n";
    First = true;
    OS << "Uses (PHI excluded): ";
    std::for_each(UsesVec.begin(), UsesVec.end(), PrintNext);
    OS << "\n";

    auto PrintNextMBB = [&OS, &First](const MachineBB *MBB) {
        if (First) First = false;
        else OS << ", ";
        OS << MBB->getReferenceName();
    };

    First = true;
    OS << "Successors: ";
    std::for_each(succ_begin(), succ_end(), PrintNextMBB);
    OS << "\n";

    First = true;
    OS << "Predecessors: ";
    std::for_each(pred_begin(), pred_end(), PrintNextMBB);
    OS << "\n";

    First = true;
    OS << "LiveIns: ";
    // const auto &LiveInsVec = ComputeLiveIns(*this);
    std::for_each(LiveIns.begin(), LiveIns.end(), PrintNext);
    OS << "\n";

    First = true;
    OS << "LiveOuts: ";
    // const auto &LiveOutsVec = ComputeLiveOuts(*this);
    std::for_each(LiveOuts.begin(), LiveOuts.end(), PrintNext);
    OS << "\n";

    OS << getReferenceName();
    OS << ":\n";
    for (const MachineInst &MI : Instructions) {
        OS << "    " << MI << "\n";
    }
}

std::list<MachineBB *> MachineBB::getSuccessors() const { return Successors; }
std::list<MachineBB *> MachineBB::getPredecessors() const { return Predecessors; }

MachineBB::iterator MachineBB::begin() { return Instructions.begin(); }
MachineBB::iterator MachineBB::end()   { return Instructions.end(); }

MachineBB::const_iterator MachineBB::begin() const { return Instructions.begin(); }
MachineBB::const_iterator MachineBB::end()   const { return Instructions.end(); }

MachineBB::reverse_iterator MachineBB::rbegin() { return Instructions.rbegin(); }
MachineBB::reverse_iterator MachineBB::rend()   { return Instructions.rend(); }

MachineBB::const_reverse_iterator MachineBB::rbegin() const { return Instructions.rbegin(); }
MachineBB::const_reverse_iterator MachineBB::rend()   const { return Instructions.rend(); }

MachineBB::succ_iterator MachineBB::succ_begin() { return Successors.begin(); }
MachineBB::succ_iterator MachineBB::succ_end()   { return Successors.end(); }

MachineBB::const_succ_iterator MachineBB::succ_begin() const { return Successors.begin(); }
MachineBB::const_succ_iterator MachineBB::succ_end()   const { return Successors.end(); }

MachineBB::pred_iterator MachineBB::pred_begin() { return Predecessors.begin(); }
MachineBB::pred_iterator MachineBB::pred_end()   { return Predecessors.end(); }

MachineBB::const_pred_iterator MachineBB::pred_begin() const { return Predecessors.begin(); }
MachineBB::const_pred_iterator MachineBB::pred_end()   const { return Predecessors.end(); }

bool isCriticalEdge(const MachineBB &From, const MachineBB &To) {
    assert(From.getMF() == To.getMF());

    // if no edge at all - it is not critical
    if (std::find(From.getSuccessors().begin(), From.getSuccessors().end(), &To) == From.getSuccessors().end()) return false;

    if (From.getSuccessors().size() > 1 && To.getPredecessors().size() > 1) return true;
    return false;
}

MachineBB *splitEdge(MachineBB &From, MachineBB &To) {
    assert(From.getMF() == To.getMF());

    MachineFunction *MF = From.getMF();
    MachineBB *SplitMBB = MF->createMBB(
        std::string("split mbb at ")
        + std::string(From.getReferenceName())
        + " -> "
        + std::string(To.getReferenceName()));

    // note: add/remove is done both ways
    From.addSuccessor(SplitMBB);
    From.removeSuccessor(&To);
    To.addPredecessor(SplitMBB);

    SplitMBB->createMI(RISCVOpcode::JAL).addReg(RISCV::RISCVRegister::ZERO).addMBB(&To);

    for (auto &MI : From) {
        // find terminator...
        if (!isControlTransferInst(MI.getOpcode())) continue;
        // in terminator replace current MBB -> SplitMBB
        for (auto &MO : MI.getOperands()) {
            if (!MO.isMBB()) continue;
            if (MO.getMBB() == &To) {
                MO.setMBB(SplitMBB);
            }
        }
    }

    return SplitMBB;
}

} // namespace Balance
