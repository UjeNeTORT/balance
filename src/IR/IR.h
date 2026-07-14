#ifndef IR_IR_H_
#define IR_IR_H_

#include "BasicBlock.h"
#include "Instruction.h"

#include <list>

namespace Balance {

class IR {
public:

private:
    std::list<BasicBlock> BasicBlocks;
};



} // Balance

#endif // IR_IR_H_
