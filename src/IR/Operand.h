#ifndef IR_OPERAND_H_
#define IR_OPERAND_H_

#include <cstddef>

namespace Balance {

class Instruction;

struct VirtRegister {
    enum RegType {
        Int, Float
    };
    RegType Type;
    size_t Id;
};

} // Balance

#endif // IR_OPERAND_H_
