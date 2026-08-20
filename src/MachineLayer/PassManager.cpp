#include "PassManager.h"
#include <memory>

using namespace Balance;

// return true if modified
bool PassManager::run(MachineFunction &MF) {
    if (MF.isDecl())
        return false;

    bool Modified = false;
    for (const auto &Pass: PassRegistry) {
        Modified |= Pass->run(MF);
    }

    // FIXME: clear passes between runs

    return Modified;
}

PassManager &PassManager::registerPass(std::unique_ptr<Pass> Pass) {
    PassRegistry.push_back(std::move(Pass));
    return *this;
}
