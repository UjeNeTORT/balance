#include "Utils.h"

#include <iostream>
#include <string>

namespace Balance {
void unreachable_internal(const std::string &File, int Line, const std::string &Msg) {
    std::cerr << "Unreachable at " << File << ":" << Line;
    if ( !Msg.empty() ) {
        std::cerr << '\n' << Msg << '\n';
    }
    exit(1);
}

}
