#ifndef MIR_PASSES_PHI_ELIMINATION_H
#define MIR_PASSES_PHI_ELIMINATION_H

#include "MIRPass.h"
#include "MachineBB.h"
#include "MachineFunction.h"

#include <string>
#include <list>
#include <tuple>

namespace Balance {

class PhiElimination final : public MIRPass {
public:
    PhiElimination(const std::string &Name = "PhiElimination") : MIRPass(Name) {}

    bool run(MachineFunction &MF) override;

private:
    using CopyCandidatesT = std::list<std::tuple<Register, Register, MachineBB *>>;

    bool prepareEliminatePHI(MachineBB &MBB, MachineInst &PHI, CopyCandidatesT &CopyCandidates);
    bool createCopy(Register To, Register From, MachineBB *MBB);
};

} // namespace Balance

#endif // MIR_PASSES_PHI_ELIMINATION_H
