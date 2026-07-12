#include <exception>
#include <iostream>
#include <string>

#include "AST/AstDotDumper.h"
#include "AST/AstDumper.h"
#include "Frontend/Driver.h"

int main(int argc, char** argv)
{
    bool dotOutput = false;
    std::string fileName;

    int argIndex = 1;

    if (argIndex < argc && std::string(argv[argIndex]) == "--dot")
    {
        dotOutput = true;
        ++argIndex;
    }

    if (argIndex < argc)
        fileName = argv[argIndex];

    Balance::Driver driver;

    int status = 0;

    try
    {
        // No file name: read from stdin.
        status = driver.parse(fileName);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    if (status != 0)
        return status;

    if (dotOutput)
    {
        Balance::AST::AstDotDumper dumper(std::cout);
        dumper.dump(*driver.getCompUnit());
    }
    else
    {
        Balance::AST::AstDumper dumper(std::cout);
        driver.getCompUnit()->accept(dumper);
    }

    return 0;
}
