#include "MachineBB.h"

namespace Balance {

MachineBB::MachineBB(MachineFunction *MF, std::string Name) : MF(MF), Name(Name), LabelIdx(MF->getNewMBBIdx()) {}

void MachineBB::insertMI(MachineBB::iterator I, MachineInst *MI) {
    Instructions.insert(I, MI);
    MI->getMBB() = this;
};

void MachineBB::insertMI(MachineInst *MI) { insertMI(end(), MI); };

MachineBB::iterator MachineBB::begin() { return Instructions.begin(); }
MachineBB::iterator MachineBB::end()   { return Instructions.end(); }

MachineBB::const_iterator MachineBB::begin() const { return Instructions.begin(); }
MachineBB::const_iterator MachineBB::end()   const { return Instructions.end(); }

int MachineBB::getLabelIdx() const { return LabelIdx; }
void MachineBB::setLabelIdx(int LabelIdxNew) { LabelIdx = LabelIdxNew; }

void MachineBB::addSuccessor(MachineBB *Succ) { Successors.push_back(Succ); }
void MachineBB::addPredecessor(MachineBB *Pred) { Predecessors.push_back(Pred); }

MachineBB::succ_iterator MachineBB::succ_begin() { return Successors.begin(); }
MachineBB::succ_iterator MachineBB::succ_end()   { return Successors.end(); }

MachineBB::const_succ_iterator MachineBB::succ_begin() const { return Successors.begin(); }
MachineBB::const_succ_iterator MachineBB::succ_end()   const { return Successors.end(); }

MachineBB::pred_iterator MachineBB::pred_begin() { return Predecessors.begin(); }
MachineBB::pred_iterator MachineBB::pred_end()   { return Predecessors.end(); }

MachineBB::const_pred_iterator MachineBB::pred_begin() const { return Predecessors.begin(); }
MachineBB::const_pred_iterator MachineBB::pred_end()   const { return Predecessors.end(); }

MachineFunction *MachineBB::getMF() const { return MF; }
void MachineBB::setMF(MachineFunction *NewMF) { MF = NewMF; }

void MachineBB::printReferenceName(std::ostream &OS) const {
    OS << "MBB." << LabelIdx;
}

void MachineBB::print(std::ostream &OS) const {
    if (!Name.empty()) OS << "\"" << Name << "\"\n";
    printReferenceName(OS);
    OS << ":\n";
    for (const MachineInst *MI : Instructions) {
        OS << "    " << *MI << "\n";
    }
}

} // namespace Balance
