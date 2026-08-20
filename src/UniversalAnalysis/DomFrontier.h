#ifndef UNIVERSAL_ANALYSIS_DOMFRONTIER_H
#define UNIVERSAL_ANALYSIS_DOMFRONTIER_H

#include <set>
#include <map>

namespace Balance {

template <typename FuncTy, typename BBTy>
class DomFrontier final {
    using NodeSetTy = std::set<BBTy *>;
    std::map<BBTy *, NodeSetTy> DomFront;

public:
    explicit DomFrontier(FuncTy &F);
    NodeSetTy getFrontier(BBTy *BB) const;
    NodeSetTy getFrontier(NodeSetTy &BBSet) const;

    NodeSetTy getIteratedFrontier(NodeSetTy &BBSet);
private:
    void compute(FuncTy &MF);
};

} // namespace Balance

#endif // UNIVERSAL_ANALYSIS_DOMFRONTIER_H
