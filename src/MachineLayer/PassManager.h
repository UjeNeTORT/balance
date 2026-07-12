#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include "MIRPasses/Pass.h"

#include <memory>
#include <vector>

namespace Balance {

class PassManager final {
    std::vector<std::unique_ptr<Pass>> PassRegistry;
public:
    bool run(MachineFunction &MF);
    PassManager &registerPass(std::unique_ptr<Pass> Pass);

    template <typename PassT, typename... Args>
    PassManager &registerPass(Args&&... args) {
        PassRegistry.push_back(std::make_unique<PassT>(std::forward<Args>(args)...));
        return *this;
}
};

} // namespace Balance

#endif // PASS_MANAGER_H
