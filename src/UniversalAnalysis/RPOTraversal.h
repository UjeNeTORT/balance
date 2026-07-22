#ifndef TRAVERSALS_RPO_TRAVERSAL_H
#define TRAVERSALS_RPO_TRAVERSAL_H

#include <list>
#include <unordered_set>

namespace Balance {

template<typename BBTy, typename FuncTy>
class RPOTraversal final {
    std::list<BBTy *> RPO;
    std::list<BBTy *> PO;

    void dfs(BBTy *Block, std::unordered_set<BBTy *> &Visited);

public:
    using iterator = typename std::list<BBTy *>::iterator;
    using const_iterator = typename std::list<BBTy *>::const_iterator;

    explicit RPOTraversal(FuncTy &Func);

    void compute(FuncTy &Func);

    std::list<BBTy *> getRPO() && { return RPO; }
    std::list<BBTy *> getPO() &&  { return PO; }

    iterator       begin()       { return RPO.begin(); }
    iterator       end()         { return RPO.end(); }
    const_iterator begin() const { return RPO.begin(); }
    const_iterator end()   const { return RPO.end(); }
};

} // namespace Balance

#endif // TRAVERSALS_RPO_TRAVERSAL_H
