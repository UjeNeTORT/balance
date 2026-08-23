#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "AST/AstDotDumper.h"
#include "AST/AstDumper.h"
#include "Frontend/Driver.h"
#include "IR2MIR/MIRBuilder.h"

#include "MachineLayer/AsmEmitter.h"
#include "MachineLayer/PassManager.h"

#include "MachineLayer/MIRPasses/VerifierPass.h"
#include "MachineLayer/MIRPasses/LivenessAnalysis.h"
#include "MachineLayer/MIRPasses/PhiElimination.h"
#include "MachineLayer/MIRPasses/LinearScanRAL.h"
#include "MachineLayer/MIRPasses/DCE.h"

using namespace Balance;
using namespace AST;

int main(int argc, char** argv) {
    bool dotOutput = false;
    std::string fileName;
    std::string outFileName;

    int argIndex = 1;

    if (argIndex < argc && std::string(argv[argIndex]) == "--dot") {
        dotOutput = true;
        ++argIndex;
    }

    if (argIndex < argc)
        fileName = argv[argIndex++];

    if (argIndex < argc)
        outFileName = argv[argIndex++];

    Balance::Driver driver;

    int status = 0;

    try {
        // No file name: read from stdin.
        status = driver.parse(fileName);
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    if (status != 0)
        return status;

    driver.foldConstants();

    if (dotOutput) {
        Balance::AST::AstDotDumper dumper(std::cout);
        dumper.dump(*driver.getCompUnit());
    } else {
        Balance::AST::AstDumper dumper(std::cout);
        driver.getCompUnit()->accept(dumper);
    }

    IR Ir = driver.buildIR();

    Ir.verify();

    MIR Mir = MIRBuilder(std::move(Ir)).build();

    PassManager PM;
    PM.registerPass<VerifierPass>();
    PM.registerPass<DeadCodeElimination>();
    PM.registerPass<VerifierPass>();
    PM.registerPass<PhiElimination>();
    PM.registerPass<VerifierPass>();
    PM.registerPass<LivenessAnalysis>();
    PM.registerPass<VerifierPass>();
    PM.registerPass<LinearScanRAL>();

    auto* Main = Mir.findFunction("main");

    for (auto& Func: Mir)
        Func.print(std::cout);

    PM.run(*Main);

    for (auto& Func: Mir)
        Func.print(std::cout);

    std::ofstream OS;
    OS.open(outFileName);
    AsmEmitter(Mir, OS).emit();
    OS.close();

    return 0;
}
