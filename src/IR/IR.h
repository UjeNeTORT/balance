#ifndef IR_IR_H_
#define IR_IR_H_

#include "BasicBlock.h"
#include "Instruction.h"

#include <list>
#include <string>

namespace Balance {

class Function {
public:
    using BasicBlockStorage = std::list<BasicBlock>;
    using iterator = BasicBlockStorage::iterator;
    using const_iterator = std::list<BasicBlock>::const_iterator;

    Function(std::string Name)
        : Name(Name)
    {}

    iterator begin() { return BasicBlocks.begin(); }
    iterator end()   { return BasicBlocks.end(); }

    const_iterator begin() const { return BasicBlocks.cbegin(); }
    const_iterator end()   const { return BasicBlocks.cend(); }
private:
    std::string Name;
    std::list<BasicBlock> BasicBlocks;
};

class IR {
public:
    using FunctionStorage = std::vector<Function>;
    using iterator = FunctionStorage::iterator;
    using const_iterator = FunctionStorage::const_iterator;

    iterator begin() { return Functions.begin(); }
    iterator end()   { return Functions.end(); }

    const_iterator begin() const { return Functions.cbegin(); }
    const_iterator end()   const { return Functions.cend(); }
private:
    FunctionStorage Functions;
};



} // Balance

#endif // IR_IR_H_
