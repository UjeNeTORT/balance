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

template<typename FuncTy, typename BBTy, typename InstTy>
class DomTree final {
    using NodeSetTy = std::set<const BBTy *>;
    NodeSetTy NodeSet;
    NodeSetTy Worklist;
    std::map<const BBTy *, NodeSetTy> DomMap;

public:
    explicit DomTree(const FuncTy &MF);

    // true if A dominates B
    bool dom(const BBTy *MBBA, const BBTy *MBBB) const;
    // true if A dominates B
    bool dom(const InstTy *MIA, const InstTy *MIB) const;

    // true if A dominates B and A != B
    bool sdom(const BBTy *MBBA, const BBTy *MBBB) const;
    // true if A dominates B and A != B
    bool sdom(const InstTy *MIA, const InstTy *MIB) const;

    // true if A sdom B and A belongs to Preds(B)
    bool idom(const BBTy *MBBA, const BBTy *MBBB) const;

    void print(std::ostream &OS) const;
private:
    void compute(const FuncTy &MF);
    void refillNodeSet(const FuncTy &MF);
    NodeSetTy computeDomIntersection(const std::list<BBTy *> &Preds) const;
};

} // namespace Balance

#endif // ANALYSIS_DOMTREE_H
