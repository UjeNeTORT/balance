#ifndef PASS_PASS_H
#define PASS_PASS_H

#include <iostream>
#include <string>

namespace Balance {

template<typename FuncTy>
class Pass {
    std::string Name;
public:
    explicit Pass(const std::string &Name) : Name(Name) {}

    virtual bool run(FuncTy &F) = 0;
    const std::string &getName() const { return Name; }
    std::ostream &dbg(std::ostream &OS = std::cerr) {
        OS << '[' << getName() << "] ";
        return OS;
    }

    virtual ~Pass() = default;
};
}

#endif // PASS_PASS_H
