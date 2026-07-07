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

  MachineFunction *MF;

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

  int getLabelIdx() const { return LabelIdx; }
  void setLabelIdx(int LabelIdxNew) { LabelIdx = LabelIdxNew; }

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
