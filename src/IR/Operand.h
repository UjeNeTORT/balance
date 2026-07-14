#ifndef IR_OPERAND_H_
#define IR_OPERAND_H_

#include <cstddef>

namespace Balance {

class Instruction;

using VirtRegister = size_t;

struct IntVirtRegister {
    VirtRegister Id;
};

struct FloatVirtRegister {
    VirtRegister Id;
};

} // Balance

#endif // IR_OPERAND_H_
