#include "PassManager.h"

using namespace Balance;

// return true if modified
bool PassManager::run(MachineFunction &MF) {
    bool Modified = false;
    for (const auto &Pass: PassRegistry) {
        Modified |= Pass->run(MF);
    }

    return Modified;
}

PassManager &PassManager::registerPass(std::unique_ptr<Pass> Pass) {
    PassRegistry.push_back(std::move(Pass));
    return *this;
}
