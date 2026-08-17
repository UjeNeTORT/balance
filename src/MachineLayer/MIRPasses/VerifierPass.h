#ifndef MIR_PASSES_VERIFIER_PASS_H
#define MIR_PASSES_VERIFIER_PASS_H

#include "MachineFunction.h"
#include "MIRPass.h"

#include <iostream>
namespace Balance {

class VerifierPass final : public MIRPass {
public:
    VerifierPass(const std::string Name = "Verifier") : MIRPass(Name) {}

    bool run(MachineFunction &MF) override;

    bool verifyCFG(MachineFunction &MF, std::string &Msg) const;
    bool verifyMBB(MachineFunction &MF, std::string &Msg) const;
    bool verifyMIDefsUses(MachineFunction &MF, std::string &Msg) const;
    bool verifySingleEntryMBB(MachineFunction &MF, std::string &Msg) const;

    void reportSuccess(std::ostream &OS = std::cerr, const std::string &Msg = "");
    void reportFail(std::ostream &OS = std::cerr, const std::string &Msg = "");
    void reportFailAndTerminate(std::ostream &OS = std::cerr, const std::string &Msg = "");
};

} // namespace Balance


#endif // MIR_PASSES_VERIFIER_PASS_H
