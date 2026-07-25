#include "Frontend/Driver.h"

#include <stdexcept>

namespace Balance
{

int Driver::parse(const std::string& fileName)
{
    File = fileName;

    Location.initialize(&File);

    scanBegin();

    yy::parser parser(*this);

    int status = parser();

    scanEnd();

    return status;
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
