#include "MachineFunction.h"
#include "MachineBB.h"

#include <list>
#include <vector>
#include <unordered_set>

namespace Balance {

class MachineBB;

class RPOTraversal {
    std::list<MachineBB *> RPO;
    std::list<MachineBB *> PO;

    void dfs(MachineBB *MBB, std::unordered_set<MachineBB *> &Visited);

public:
    using iterator = std::list<MachineBB *>::iterator;
    using const_iterator = std::list<MachineBB *>::const_iterator;

    explicit RPOTraversal(MachineFunction &MF);

    void compute(MachineFunction &MF);
    iterator begin();
    iterator end();

    std::list<MachineBB *> getRPO() &&;
    std::list<MachineBB *> getPO() &&;

    const_iterator begin() const;
    const_iterator end()   const;
};

} // namespace Balance
