#ifndef PASS_PASS_MANAGER_H
#define PASS_PASS_MANAGER_H

#include "Pass/Pass.h"

#include <memory>
#include <utility>
#include <vector>

namespace Balance {

template <typename FuncTy>
class FunctionPassManager final {
    std::vector<std::unique_ptr<Pass<FuncTy>>> PassRegistry;

public:
    bool run(FuncTy &F) {
        bool Modified = false;
        for (const auto &RegisteredPass: PassRegistry)
            Modified |= RegisteredPass->run(F);

        return Modified;
    }

    FunctionPassManager &registerPass(std::unique_ptr<Pass<FuncTy>> RegisteredPass) {
        PassRegistry.push_back(std::move(RegisteredPass));
        return *this;
    }

    template <typename PassT, typename... Args>
    FunctionPassManager &registerPass(Args&&... args) {
        PassRegistry.push_back(std::make_unique<PassT>(std::forward<Args>(args)...));
        return *this;
    }
};

} // namespace Balance

#endif // PASS_PASS_MANAGER_H
