#ifndef UNIVERSAL_ANALYSIS_DOMFRONTIER_H
#define UNIVERSAL_ANALYSIS_DOMFRONTIER_H

#include <set>
#include <map>

namespace Balance {

template <typename FuncTy, typename BBTy>
class DomFrontier final {
    using NodeSetTy = std::set<const BBTy *>;
    std::map<const BBTy *, NodeSetTy> DomFront;

public:
    explicit DomFrontier(FuncTy &F);
    const NodeSetTy &getFrontier(const BBTy *BB);
private:
    void compute(const FuncTy &MF);
};

} // namespace Balance

#endif // UNIVERSAL_ANALYSIS_DOMFRONTIER_H
