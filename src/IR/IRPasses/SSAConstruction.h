#ifndef IR_IR_PASSES_SSA_CONSTRUCTION_H
#define IR_IR_PASSES_SSA_CONSTRUCTION_H

#include "IRPass.h"

namespace Balance {

class SSAConstruction final : public IRPass {

public:
    SSAConstruction(const std::string &Name = "ssa-construction") : IRPass(Name) {}

    bool run(Function &F) override;
};
}

#endif // IR_IR_PASSES_SSA_CONSTRUCTION_H
