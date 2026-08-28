#include <cxxopts.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "AST/AstDotDumper.h"
#include "AST/AstDumper.h"
#include "Frontend/Driver.h"
#include "IR2MIR/MIRBuilder.h"

#include "MachineLayer/AsmEmitter.h"

using namespace Balance;
using namespace AST;

int main(int argc, char** argv) {
    cxxopts::Options Options("balancc", "Balance compiler");

    Options.add_options()
        ("S,asm", "Generate assembly", cxxopts::value<bool>()->default_value("true"))
        ("o,output", "Output file path", cxxopts::value<std::filesystem::path>()->
                                                  default_value("a.s"))
        ("d,dot", "Enable dot output")
        ("h,help", "Print help")
    ;

    Options.positional_help("<file>");
    Options.show_positional_help();
    auto OptRes = Options.parse(argc, argv);

    if (OptRes.count("help")) {
        std::cout << Options.help() << std::endl;
        return 0;
    }

    const auto& OptUnmatched = OptRes.unmatched();

    if (OptUnmatched.size() != 1) {
        std::cout << "error: no input file or too many given" << std::endl;
        return 1;
    }

    Driver Driver;

    Driver.parse(OptUnmatched[0]);

    Driver.foldConstants();

    Balance::AST::AstDotDumper Dumper(std::cout);
    if (OptRes.count("dot"))
        Dumper.dump(*Driver.getCompUnit());
    else
        Driver.getCompUnit()->accept(Dumper);

    IR Ir = Driver.buildIR();

    Ir.verify();

    MIR Mir = MIRBuilder(std::move(Ir)).build();

    for (auto& Func: Mir)
        Func.print(std::cout);

    Mir.runPasses();

    for (auto& Func: Mir)
        Func.print(std::cout);

    std::ofstream OS;
    OS.open(OptRes["output"].as<std::filesystem::path>());
    AsmEmitter(Mir, OS).emit();
    OS.close();

    return 0;
}
