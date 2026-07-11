#ifndef MACHINE_FUNCTION_H
#define MACHINE_FUNCTION_H

#include <memory>
#include <string>
#include <string_view>

#include <list>

namespace Balance {

class MachineBB;

class MachineFunction {
    std::list<MachineBB> BasicBlocks;
    using iterator = std::list<MachineBB>::iterator;
    using const_iterator = std::list<MachineBB>::const_iterator;

    int MBBIdx = 0; // incremented each time as a new MBB is created
    std::string Name;
public:
    MachineFunction(const std::string &Name);

    std::string_view getName() const;

    MachineBB *createMBB(const std::string &Name = "");
    MachineBB *createMBB(iterator Pos, const std::string &Name = "");

    int getNewMBBIdx();

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end()   const;

    void print(std::ostream &OS) const;
};

} // namespace Balance

#endif // MACHINE_FUNCTION_H
