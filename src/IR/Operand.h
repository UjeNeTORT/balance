#ifndef IR_OPERAND_H_
#define IR_OPERAND_H_

#include <cstddef>
#include <optional>

namespace Balance {

class BasicBlock;
class Instruction;

struct VirtRegister {
    enum RegType {
        Int, Float
    } Type;
    size_t Id;

    std::optional<BasicBlock*> DefBlock;
};

} // Balance

#endif // IR_OPERAND_H_
