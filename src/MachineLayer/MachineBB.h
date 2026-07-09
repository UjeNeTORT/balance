#ifndef MACHINE_BB_H
#define MACHINE_BB_H

#include "MachineInst.h"
#include "MachineFunction.h"

#include <list>
#include <iostream>

namespace Balance {

class MachineBB {
    std::list<MachineInst *> Instructions;
    using iterator = std::list<MachineInst *>::iterator;
    using const_iterator = std::list<MachineInst *>::const_iterator;

    MachineFunction *MF;

    std::string Name = ""; // mostly for comments & debug

    std::vector<MachineBB *> Successors;
    using succ_iterator = std::vector<MachineBB *>::iterator;
    using const_succ_iterator = std::vector<MachineBB *>::const_iterator;

    std::vector<MachineBB *> Predecessors;
    using pred_iterator = std::vector<MachineBB *>::iterator;
    using const_pred_iterator = std::vector<MachineBB *>::const_iterator;

    int LabelIdx = -1; // used for asm printing
public:
    MachineBB(MachineFunction *MF, std::string Name = "") : MF(MF), Name(Name), LabelIdx(MF->getNewMBBIdx()) {}

    void insertMI(MachineBB::iterator I, MachineInst *MI) {
        Instructions.insert(I, MI);
        MI->getMBB() = this;
    };

    void insertMI(MachineInst *MI) { insertMI(end(), MI); };

    iterator begin() { return Instructions.begin(); }
    iterator end()   { return Instructions.end(); }

    const_iterator begin() const { return Instructions.begin(); }
    const_iterator end()   const { return Instructions.end(); }

    int getLabelIdx() const { return LabelIdx; }
    void setLabelIdx(int LabelIdxNew) { LabelIdx = LabelIdxNew; }

    void addSuccessor(MachineBB *Succ) { Successors.push_back(Succ); }
    void addPredecessor(MachineBB *Pred) { Predecessors.push_back(Pred); }

    succ_iterator succ_begin() { return Successors.begin(); }
    succ_iterator succ_end()   { return Successors.end(); }

    const_succ_iterator succ_begin() const { return Successors.begin(); }
    const_succ_iterator succ_end()   const { return Successors.end(); }

    pred_iterator pred_begin() { return Predecessors.begin(); }
    pred_iterator pred_end()   { return Predecessors.end(); }

    const_pred_iterator pred_begin() const { return Predecessors.begin(); }
    const_pred_iterator pred_end()   const { return Predecessors.end(); }

    MachineFunction *getMF() const { return MF; }
    void setMF(MachineFunction *NewMF) { MF = NewMF; }

    void printReferenceName(std::ostream &OS) const {
        OS << "MBB." << LabelIdx;
    }

    void print(std::ostream &OS) const {
        if (!Name.empty()) OS << "\"" << Name << "\"\n";
        printReferenceName(OS);
        OS << ":\n";
        for (const MachineInst *MI : Instructions) {
            OS << "    " << *MI << "\n";
        }
    }
};

} // namespace Balance

#endif // MACHINE_BB_H
