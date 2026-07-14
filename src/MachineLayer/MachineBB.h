#ifndef MACHINE_BB_H
#define MACHINE_BB_H

#include "MIROpcodes.h"
#include "Register.h"
#include "MachineInst.h"

#include <iostream>
#include <list>
#include <unordered_set>

namespace Balance {

class MachineFunction;

class MachineBB {
    MachineFunction *MF;

    int LabelIdx = -1; // used for asm printing

    std::string Name = ""; // mostly for comments & debug
    std::string ReferenceName = ""; // for asm printing

    std::unordered_set<Register> LiveIns;
    std::unordered_set<Register> LiveOuts;

    std::list<MachineInst> Instructions;

    std::list<MachineBB *> Successors;
    std::list<MachineBB *> Predecessors;
public:
    using iterator = std::list<MachineInst>::iterator;
    using const_iterator = std::list<MachineInst>::const_iterator;
    using succ_iterator = std::list<MachineBB *>::iterator;
    using const_succ_iterator = std::list<MachineBB *>::const_iterator;
    using pred_iterator = std::list<MachineBB *>::iterator;
    using const_pred_iterator = std::list<MachineBB *>::const_iterator;

    MachineBB(MachineFunction *MF, std::string Name = "");

    MachineInst &createMI(MachineBB::iterator I, RISCVOpcode Opcode);
    MachineInst &createMI(RISCVOpcode Opcode);

    MachineInst &insertMI(MachineBB::iterator I, MachineInst MI);
    MachineInst &insertMI(MachineInst MI);

    iterator eraseMI(MachineBB::iterator I);

    int getLabelIdx() const;
    void setLabelIdx(int LabelIdxNew);

    const std::unordered_set<Register> &getLiveIns() const;
    const std::unordered_set<Register> &getLiveOuts() const;

    void setLiveIns(const std::unordered_set<Register> &LiveInsNew);
    void setLiveOuts(const std::unordered_set<Register> &LiveOutsNew);

    void addSuccessor(MachineBB *Succ);
    void addPredecessor(MachineBB *Pred);
    void addSuccessorOneWay(MachineBB *Succ);
    void addPredecessorOneWay(MachineBB *Pred);

    void removeSuccessor(MachineBB *Succ);
    void removePredecessor(MachineBB *Pred);
    void removeSuccessorOneWay(MachineBB *Succ);
    void removePredecessorOneWay(MachineBB *Pred);

    MachineFunction *getMF() const;
    void setMF(MachineFunction *NewMF);

    bool verify() const;

    std::string_view getReferenceName() const;
    void updateReferenceName();
    void print(std::ostream &OS) const;

    std::list<MachineBB *> getSuccessors() const;
    std::list<MachineBB *> getPredecessors() const;

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

bool isCriticalEdge(const MachineBB &From, const MachineBB &To);
MachineBB *splitEdge(MachineBB &From, MachineBB &To);

} // namespace Balance

#endif // MACHINE_BB_H
