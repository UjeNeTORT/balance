#include "MachineBB.h"
#include "LiveRanges.h"

#include <algorithm>
#include <cassert>

namespace Balance {

MachineBB::MachineBB(MachineFunction *MF, std::string Name) : MF(MF), Name(Name), LabelIdx(MF->getNewMBBIdx()) {}

MachineInst &MachineBB::createMI(MachineBB::iterator I, RISCVOpcode Opcode) {
    MachineInst *MI = &*Instructions.emplace(I, Opcode);
    MI->setMBB(this);
    if (isControlTransferInst(*MI)) {
        std::cerr << "WARN: added control transfer to MBB, its destination is unknown, MBB succ and preds not updated!\n";
        // TODO:
    }
    return *MI;
};

MachineInst &MachineBB::createMI(RISCVOpcode Opcode) {
    return createMI(end(), Opcode);
};

MachineInst *MachineBB::insertMI(MachineBB::iterator I, MachineInst MI) {
    MI.setMBB(this);
    I = Instructions.insert(I, MI);
    if (isControlTransferInst(MI)) {
        I->getMBB()->addPredecessor(this);
        addSuccessor(I->getMBB());
    }
    return &*--I;
};

MachineInst *MachineBB::insertMI(MachineInst MI) {
    return insertMI(end(), MI);
};


int MachineBB::getLabelIdx() const { return LabelIdx; }
void MachineBB::setLabelIdx(int LabelIdxNew) { LabelIdx = LabelIdxNew; }

void MachineBB::addSuccessor(MachineBB *Succ) {
    Successors.push_back(Succ);
}
void MachineBB::addPredecessor(MachineBB *Pred) {
    Predecessors.push_back(Pred);
}

bool MachineBB::verify() const {
    bool Valid = true;
    std::for_each(Successors.begin(), Successors.end(), [&Valid, this](MachineBB *Succ) {
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

void MachineBB::printReferenceName(std::ostream &OS) const {
    OS << "MBB." << LabelIdx;
}

void MachineBB::print(std::ostream &OS) const {
    if (!Name.empty()) OS << "\"" << Name << "\"\n";

    auto &&DefsVec = Defs(*this);
    auto &&UsesVec = Uses(*this);

    bool First = true;
    auto PrintNextMO = [&OS, &First](const Register &Reg) {
        if (First) First = false;
        else OS << ", ";
        OS << Reg;
    };

    OS << "Defs: ";
    std::for_each(DefsVec.begin(), DefsVec.end(), PrintNextMO);
    OS << "\n";
    First = true;
    OS << "Uses: ";
    std::for_each(UsesVec.begin(), UsesVec.end(), PrintNextMO);
    OS << "\n";

    printReferenceName(OS);
    OS << ":\n";
    for (const MachineInst &MI : Instructions) {
        OS << "    " << MI << "\n";
    }
}

MachineBB::iterator MachineBB::begin() { return Instructions.begin(); }
MachineBB::iterator MachineBB::end()   { return Instructions.end(); }

MachineBB::const_iterator MachineBB::begin() const { return Instructions.begin(); }
MachineBB::const_iterator MachineBB::end()   const { return Instructions.end(); }

MachineBB::succ_iterator MachineBB::succ_begin() { return Successors.begin(); }
MachineBB::succ_iterator MachineBB::succ_end()   { return Successors.end(); }

MachineBB::const_succ_iterator MachineBB::succ_begin() const { return Successors.begin(); }
MachineBB::const_succ_iterator MachineBB::succ_end()   const { return Successors.end(); }

MachineBB::pred_iterator MachineBB::pred_begin() { return Predecessors.begin(); }
MachineBB::pred_iterator MachineBB::pred_end()   { return Predecessors.end(); }

MachineBB::const_pred_iterator MachineBB::pred_begin() const { return Predecessors.begin(); }
MachineBB::const_pred_iterator MachineBB::pred_end()   const { return Predecessors.end(); }

} // namespace Balance
