#ifndef UNIVERSAL_ANALYSIS_DOMTREE_H
#define UNIVERSAL_ANALYSIS_DOMTREE_H

#include <list>
#include <map>
#include <ostream>
#include <set>
#include <type_traits>

namespace Balance {

class MachineBB;
class MachineFunction;
class MachineInst;

template <typename T, typename = void>
struct has_getParent : std::false_type {};

template <typename T>
struct has_getParent<T, std::void_t<decltype(std::declval<T>().getParent())>>
    : std::true_type {};

template<typename FuncTy, typename BBTy, typename InstTy>
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
    template <typename T = InstTy>
    auto dom(const InstTy *IA, const InstTy *IB) const
        -> std::enable_if_t<has_getParent<T>::value, bool> {
        assert(DomMap.find(IA->getParent()) != DomMap.end() && "Unknown Dom[IA->getParent()]");
        assert(DomMap.find(IB->getParent()) != DomMap.end() && "Unknown Dom[IB->getParent()]");

        const BBTy *ParentA = IA->getParent();
        const BBTy *ParentB = IB->getParent();

        if (sdom(ParentA, ParentB)) return true;
        if (ParentA == ParentB) {
            for (auto It = ParentA->begin(), Ie = ParentA->end(); It != Ie; ++It) {
                const auto &MI = *It;
                // now look who is first in parent mbb
                // if A => then A dom B = true
                if (&MI == IA) return true;
                if (&MI == IB) return false;
            }

        }

        return false;
    }

    // true if A dominates B and A != B
    bool sdom(const BBTy *BBA, const BBTy *BBB) const;
    // true if A dominates B and A != B
    template <typename T = InstTy>
    auto sdom(const InstTy *IA, const InstTy *IB) const
        -> std::enable_if_t<has_getParent<T>::value, bool> {
        return dom(IA, IB) && IA != IB;
    }

    // true if A dom B and A belongs to Preds(B) in Dom Tree
    bool idom(const BBTy *BBA, const BBTy *BBB) const;

    // return node Strict Dominators
    NodeSetTy getSDoms(const BBTy *BB);

    // return node immediate dominator
    const BBTy *getIDom(const BBTy *BB);

    void print(std::ostream &OS) const;
private:
    void compute(const FuncTy &MF);
    void refillNodeSet(const FuncTy &MF);
    NodeSetTy computeDomIntersection(const std::list<BBTy *> &Preds) const;
};

} // namespace Balance

#endif // UNIVERSAL_ANALYSIS_DOMTREE_H
