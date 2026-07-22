#ifndef MACHINE_FUNCTION_H
#define MACHINE_FUNCTION_H

// for Register, TODO: move Register to Register.h
#include "MachineOperand.h"

#include <string>
#include <string_view>

#include <list>

namespace Balance {

class MachineBB;

class MachineFunction {
    std::list<MachineBB> BasicBlocks;

    int MBBIdx = 0; // incremented each time as a new MBB is created
    int FreeVRegId = 1;
    std::string Name;
public:
    using iterator = std::list<MachineBB>::iterator;
    using const_iterator = std::list<MachineBB>::const_iterator;

    MachineFunction(const std::string &Name);

    std::string_view getName() const;

    MachineBB *entryBB();
    MachineBB *createMBB(const std::string &Name = "");
    MachineBB *createMBB(iterator Pos, const std::string &Name = "");

    Register getNewVreg();

    std::list<MachineBB> &getBasicBlocks() { return BasicBlocks; }

    int getNewMBBIdx();

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end()   const;

    void print(std::ostream &OS) const;
};

} // namespace Balance

#endif // MACHINE_FUNCTION_H
