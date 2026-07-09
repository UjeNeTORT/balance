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
    MachineBB(MachineFunction *MF, std::string Name = "");

    void insertMI(MachineBB::iterator I, MachineInst *MI);

    void insertMI(MachineInst *MI);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    int getLabelIdx() const;
    void setLabelIdx(int LabelIdxNew);

    void addSuccessor(MachineBB *Succ);
    void addPredecessor(MachineBB *Pred);

    succ_iterator succ_begin();
    succ_iterator succ_end();

    const_succ_iterator succ_begin() const;
    const_succ_iterator succ_end() const;

    pred_iterator pred_begin();
    pred_iterator pred_end();

    const_pred_iterator pred_begin() const;
    const_pred_iterator pred_end() const;

    MachineFunction *getMF() const;
    void setMF(MachineFunction *NewMF);

    void printReferenceName(std::ostream &OS) const;

    void print(std::ostream &OS) const;
};

} // namespace Balance

#endif // MACHINE_BB_H
