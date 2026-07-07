#ifndef MACHINE_FUNCTION_H
#define MACHINE_FUNCTION_H

#include <string>
#include <string_view>

#include <list>

namespace Balance {

class MachineBB;

class MachineFunction {
    std::list<MachineBB *> BasicBlocks;
    using iterator = std::list<MachineBB *>::iterator;
    using const_iterator = std::list<MachineBB *>::const_iterator;

    int MBBIdx = 0; // incremented each time as a new MBB is created
    std::string Name;

private:
    int getNewMBBIdx();

public:
    MachineFunction(std::string Name);
    std::string_view getName() const;

    void InsertMBB(MachineBB *MBB);

    void InsertMBB(iterator I, MachineBB *MBB);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end()   const;

    void print(std::ostream &OS) const;
};

} // namespace Balance

#endif // MACHINE_FUNCTION_H
