#ifndef ANALYSIS_DOMTREE_H
#define ANALYSIS_DOMTREE_H

#include <list>
#include <map>
#include <ostream>
#include <set>

namespace Balance {

class MachineBB;
class MachineFunction;
class MachineInst;

class DomTree final {
    using NodeSetTy = std::set<const MachineBB *>;
    NodeSetTy NodeSet;
    NodeSetTy Worklist;
    std::map<const MachineBB *, NodeSetTy> Dom;

public:
    explicit DomTree(const MachineFunction &MF);

    bool dominates(const MachineBB *MBBA, const MachineBB *MBBB);
    bool dominates(const MachineInst *MIA, const MachineInst *MIB);

    void print(std::ostream &OS) const;
private:
    void compute(const MachineFunction &MF);
    void refillNodeSet(const MachineFunction &MF);
    NodeSetTy computeDomIntersection(const std::list<MachineBB *> &Preds) const;
};

} // namespace Balance

#endif // ANALYSIS_DOMTREE_H
