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

    std::vector<MachineBB *> Successors;
    using succ_iterator = std::vector<MachineBB *>::iterator;
    using const_succ_iterator = std::vector<MachineBB *>::const_iterator;

    int LabelIdx = -1; // used for asm printing
public:
    MachineBB(MachineFunction *MF) : MF(MF) {}

    void InsertMI(MachineBB::iterator I, MachineInst *MI) {
        Instructions.insert(I, MI);
        MI->getMBB() = this;
    };

    void InsertMI(MachineInst *MI) { InsertMI(end(), MI); };

    iterator begin() { return Instructions.begin(); }
    iterator end()   { return Instructions.end(); }

    const_iterator begin() const { return Instructions.begin(); }
    const_iterator end()   const { return Instructions.end(); }

    int getLabelIdx() const { return LabelIdx; }
    void setLabelIdx(int LabelIdxNew) { LabelIdx = LabelIdxNew; }

    void addSuccessor(MachineBB *Succ) { Successors.push_back(Succ); }

    succ_iterator succ_begin() { return Successors.begin(); }
    succ_iterator succ_end()   { return Successors.end(); }

    const_succ_iterator succ_begin() const { return Successors.begin(); }
    const_succ_iterator succ_end()   const { return Successors.end(); }

    MachineFunction *getMF() const { return MF; }

    void printReferenceName(std::ostream &OS) const {
        OS << "MBB." << LabelIdx;
    }

    void print(std::ostream &OS) const {
        printReferenceName(OS);
        OS << ":\n";
        for (const MachineInst *MI : Instructions) {
            OS << "    " << *MI << "\n";
        }
    }
};

} // namespace Balance

#endif // MACHINE_BB_H
