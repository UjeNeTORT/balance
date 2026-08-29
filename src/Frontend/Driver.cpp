#include "Frontend/Driver.h"

#include <stdexcept>
#include <string>

namespace Balance
{

void Driver::parse(const std::string& fileName)
{
    File = fileName;

    Location.initialize(&File);

    scanBegin();

    yy::parser parser(*this);

    int status = parser();

    scanEnd();

    if (status != 0)
        throw std::runtime_error("Frontend error: " + std::to_string(status));
}

void Driver::scanBegin()
{
    if (File.empty())
        yyin = stdin;
    else if (!(yyin = fopen(File.c_str(), "r")))
        throw std::runtime_error("Can't open input file: " + File);
}

void Driver::scanEnd()
{
    if (yyin && yyin != stdin)
        fclose(yyin);
}

} // namespace Balance
