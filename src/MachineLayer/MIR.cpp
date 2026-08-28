#include "MIR.h"

#include "PassManager.h"

#include "MIRPasses/VerifierPass.h"
#include "MIRPasses/LivenessAnalysis.h"
#include "MIRPasses/PhiElimination.h"
#include "MIRPasses/LinearScanRAL.h"
#include "MIRPasses/DCE.h"

using namespace Balance;

void MIR::runPasses() {
    for (auto& Func: *this) {
        PassManager PM;
        PM.registerPass<VerifierPass>();
        PM.registerPass<DeadCodeElimination>();
        PM.registerPass<VerifierPass>();
        PM.registerPass<PhiElimination>();
        PM.registerPass<VerifierPass>();
        PM.registerPass<LivenessAnalysis>();
        PM.registerPass<VerifierPass>();
        PM.registerPass<LinearScanRAL>();

        PM.run(Func);
    }
}

