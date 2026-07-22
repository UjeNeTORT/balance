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
    std::map<const MachineBB *, NodeSetTy> DomMap;

public:
    explicit DomTree(const MachineFunction &MF);

    // true if A dominates B
    bool dom(const MachineBB *MBBA, const MachineBB *MBBB) const;
    // true if A dominates B
    bool dom(const MachineInst *MIA, const MachineInst *MIB) const;

    // true if A dominates B and A != B
    bool sdom(const MachineBB *MBBA, const MachineBB *MBBB) const;
    // true if A dominates B and A != B
    bool sdom(const MachineInst *MIA, const MachineInst *MIB) const;

    // true if A sdom B and A belongs to Preds(B)
    bool idom(const MachineBB *MBBA, const MachineBB *MBBB) const;

    void print(std::ostream &OS) const;
private:
    void compute(const MachineFunction &MF);
    void refillNodeSet(const MachineFunction &MF);
    NodeSetTy computeDomIntersection(const std::list<MachineBB *> &Preds) const;
};

} // namespace Balance

#endif // ANALYSIS_DOMTREE_H
