#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H

#include "MIRPasses/Pass.h"

#include <vector>

namespace Balance {

class PassManager final {
    std::vector<const Pass &> PassRegistry;

public:


};

} // namespace Balance

#endif // PASS_MANAGER_H
