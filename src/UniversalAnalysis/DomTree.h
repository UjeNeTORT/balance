#ifndef UNIVERSAL_ANALYSIS_DOMTREE_H
#define UNIVERSAL_ANALYSIS_DOMTREE_H

#include "IR/Instruction.h"
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <type_traits>

namespace Balance {

class MachineBB;
class MachineFunction;
class MachineInst;

template <typename T>
struct is_inst_ty : std::false_type {};
template<> struct is_inst_ty<Instruction> : std::true_type {};
template<> struct is_inst_ty<MachineInst> : std::true_type {};

using DummyT = int;
template<typename FuncTy, typename BBTy, typename InstTy = DummyT>
class DomTree final {
    using NodeSetTy = std::set<const BBTy *>;
    NodeSetTy NodeSet;
    NodeSetTy Worklist;
    std::map<const BBTy *, NodeSetTy> DomMap;
    std::map<const BBTy *, const BBTy *> IDomMap;

public:
    explicit DomTree(const FuncTy &F);

    // true if A dominates B
    bool dom(const BBTy *BBA, const BBTy *BBB) const;
    // true if A dominates B
    template <typename InstTyT = InstTy, typename = typename std::enable_if_t<is_inst_ty<InstTyT>::value>>
    bool dom(const InstTyT *IA, const InstTyT *IB) const;

    // true if A dominates B and A != B
    bool sdom(const BBTy *BBA, const BBTy *BBB) const;
    // true if A dominates B and A != B
    template <typename InstTyT = InstTy, typename = typename std::enable_if_t<is_inst_ty<InstTyT>::value>>
    bool sdom(const InstTyT *IA, const InstTyT *IB) const;

    // true if A dom B and A belongs to Preds(B) in Dom Tree
    bool idom(const BBTy *BBA, const BBTy *BBB) const;

    // return node Strict Dominators
    NodeSetTy getSDoms(const BBTy *BB);

    // return node immediate dominator
    const BBTy *getIDom(const BBTy *BB) const;
    // return node immediate dominator
    BBTy *getIDom(BBTy *BB) const;

    void print(std::ostream &OS) const;
private:
    void compute(const FuncTy &MF);
    void refillNodeSet(const FuncTy &MF);
    NodeSetTy computeDomIntersection(const std::list<BBTy *> &Preds) const;
};

} // namespace Balance

#endif // UNIVERSAL_ANALYSIS_DOMTREE_H
