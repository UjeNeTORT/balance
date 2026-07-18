#ifndef IR_IR_H_
#define IR_IR_H_

#include "BasicBlock.h"
#include "Instruction.h"

#include <iterator>
#include <list>
#include <string>
#include <utility>

namespace Balance {

class Function {
public:
    using BasicBlockStorage = std::list<BasicBlock>;
    using iterator = BasicBlockStorage::iterator;
    using const_iterator = std::list<BasicBlock>::const_iterator;

    Function(std::string Name)
        : Name(Name)
    {}

    std::pair<iterator, BasicBlock::iterator> addInstruction(Opcodes Opcode,
                std::optional<SourceInfo> SrcInfo = std::nullopt) {
        iterator LastBB = std::prev(BasicBlocks.end());
        if (!BasicBlocks.empty()) {
            if (!LastBB->empty() && std::prev(LastBB->end())->get()->isTerminal())
                LastBB = BasicBlocks.insert(BasicBlocks.end(), this);
        }
        return {LastBB, LastBB->addInstruction(Opcode, SrcInfo)};
    }

    iterator       begin()       { return BasicBlocks.begin(); }
    iterator       end()         { return BasicBlocks.end(); }
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

    iterator       begin()       { return Functions.begin(); }
    iterator       end()         { return Functions.end(); }
    const_iterator begin() const { return Functions.cbegin(); }
    const_iterator end()   const { return Functions.cend(); }
private:
    FunctionStorage Functions;
};



} // Balance

#endif // IR_IR_H_
