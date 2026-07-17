#include "MachineFunction.h"
#include "MachineInst.h"
#include "MachineBB.h"

#include <iostream>

namespace Balance {

int MachineFunction::getNewMBBIdx() { return MBBIdx++; }

MachineFunction::MachineFunction(const std::string &Name) : Name(Name) {}

std::string_view MachineFunction::getName() const { return Name; }

MachineBB *MachineFunction::createMBB(const std::string &Name) {
    BasicBlocks.emplace_back(this, Name);
    return &*--BasicBlocks.end();
}

MachineBB *MachineFunction::createMBB(MachineFunction::iterator Pos, const std::string &Name) {
    iterator MBB = BasicBlocks.emplace(Pos, this, Name);
    return &*MBB;
}

Register MachineFunction::getNewVreg() {
    return Register(FreeVRegId++);
}

MachineFunction::iterator MachineFunction::begin() { return BasicBlocks.begin(); }
MachineFunction::iterator MachineFunction::end()   { return BasicBlocks.end(); }

MachineFunction::const_iterator MachineFunction::begin() const { return BasicBlocks.begin(); }
MachineFunction::const_iterator MachineFunction::end()   const { return BasicBlocks.end(); }

void MachineFunction::print(std::ostream &OS) const {
    OS << "MachineFunction: " << Name << "\n";
    for (const auto &MBB : BasicBlocks) {
        MBB.print(OS);
        OS << "\n";
    }
}

} // namespace Balance
