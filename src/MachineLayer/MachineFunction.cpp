#include "MachineFunction.h"
#include "MachineBB.h"

#include <iostream>

namespace Balance {

int MachineFunction::getNewMBBIdx() { return MBBIdx++; }

MachineFunction::MachineFunction(std::string Name) : Name(Name) {}

std::string_view MachineFunction::getName() const { return Name; }

void MachineFunction::InsertMBB(MachineBB *MBB) {
    BasicBlocks.push_back(MBB);
    MBB->setLabelIdx(getNewMBBIdx());
}

void MachineFunction::InsertMBB(MachineFunction::iterator I, MachineBB *MBB) {
    BasicBlocks.insert(I, MBB);
    MBB->setLabelIdx(getNewMBBIdx());
}

MachineFunction::iterator MachineFunction::begin() { return BasicBlocks.begin(); }
MachineFunction::iterator MachineFunction::end()   { return BasicBlocks.end(); }

MachineFunction::const_iterator MachineFunction::begin() const { return BasicBlocks.begin(); }
MachineFunction::const_iterator MachineFunction::end()   const { return BasicBlocks.end(); }

void MachineFunction::print(std::ostream &OS) const {
    OS << "MachineFunction: " << Name << "\n";
    for (const auto &MBB : BasicBlocks) {
        MBB->print(OS);
        OS << "\n";
    }
}

} // namespace Balance
