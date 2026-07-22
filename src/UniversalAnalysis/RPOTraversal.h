#ifndef TRAVERSALS_RPO_TRAVERSAL_H
#define TRAVERSALS_RPO_TRAVERSAL_H

#include <list>
#include <unordered_set>

namespace Balance {

template<typename BB, typename F>
class RPOTraversal {
    std::list<BB *> RPO;
    std::list<BB *> PO;

    void dfs(BB *Block, std::unordered_set<BB *> &Visited);

public:
    using iterator = typename std::list<BB *>::iterator;
    using const_iterator = typename std::list<BB *>::const_iterator;

    explicit RPOTraversal(F &Func);

    void compute(F &Func);

    std::list<BB *> getRPO() && { return std::move(RPO); }
    std::list<BB *> getPO() &&  { return std::move(PO); }

    iterator       begin()       { return RPO.begin(); }
    iterator       end()         { return RPO.end(); }
    const_iterator begin() const { return RPO.begin(); }
    const_iterator end()   const { return RPO.end(); }
};

} // namespace Balance

#endif // TRAVERSALS_RPO_TRAVERSAL_H
