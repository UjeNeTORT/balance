#ifndef MACHINE_BB_H
#define MACHINE_BB_H

#include "MachineInst.h"
#include "MachineFunction.h"

#include <list>
#include <iostream>

namespace Balance {

class MachineBB {
    MachineFunction *MF;

    int LabelIdx = -1; // used for asm printing

    std::string Name = ""; // mostly for comments & debug
    mutable std::string ReferenceName = ""; // for asm printing

    std::list<MachineInst> Instructions;
    using iterator = std::list<MachineInst>::iterator;
    using const_iterator = std::list<MachineInst>::const_iterator;

    std::vector<MachineBB *> Successors;
    using succ_iterator = std::vector<MachineBB *>::iterator;
    using const_succ_iterator = std::vector<MachineBB *>::const_iterator;

    std::vector<MachineBB *> Predecessors;
    using pred_iterator = std::vector<MachineBB *>::iterator;
    using const_pred_iterator = std::vector<MachineBB *>::const_iterator;
public:
    MachineBB(MachineFunction *MF, std::string Name = "");

    MachineInst &createMI(MachineBB::iterator I, RISCVOpcode Opcode);
    MachineInst &createMI(RISCVOpcode Opcode);

    MachineInst *insertMI(MachineBB::iterator I, MachineInst MI);
    MachineInst *insertMI(MachineInst MI);

    int getLabelIdx() const;
    void setLabelIdx(int LabelIdxNew);

    void addSuccessor(MachineBB *Succ);
    void addPredecessor(MachineBB *Pred);

    MachineFunction *getMF() const;
    void setMF(MachineFunction *NewMF);

    bool verify() const;

    std::string_view getReferenceName() const;
    void updateReferenceName();
    void print(std::ostream &OS) const;

    std::vector<MachineBB *> getSuccessors() const;
    std::vector<MachineBB *> getPredecessors() const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    succ_iterator succ_begin();
    succ_iterator succ_end();

    const_succ_iterator succ_begin() const;
    const_succ_iterator succ_end() const;

    pred_iterator pred_begin();
    pred_iterator pred_end();

    const_pred_iterator pred_begin() const;
    const_pred_iterator pred_end() const;
};

} // namespace Balance

#endif // MACHINE_BB_H
