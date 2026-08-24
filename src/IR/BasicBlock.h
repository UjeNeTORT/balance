#ifndef IR_BASICBLOCK_H
#define IR_BASICBLOCK_H

#include "IR/Instruction.h"

#include <iterator>
#include <list>
#include <string>

namespace Balance {

class Function;

class BasicBlock {
public:
    using instructions_storage = std::list<Instruction>;
    using iterator = instructions_storage::iterator;
    using const_iterator = instructions_storage::const_iterator;

    using bb_storage = std::list<BasicBlock*>;
    using bb_iterator = bb_storage::iterator;
    using bb_const_iterator = bb_storage::const_iterator;

    BasicBlock(Function* Parent, std::string Name) : ParentFunction(Parent), Name(Name) {}

    BasicBlock(Function* Parent, std::string Name, SourceInfo SrcInf) :
        ParentFunction(Parent), Name(Name), SrcInfo(SrcInf) {}

    BasicBlock(const BasicBlock&) = delete;
    BasicBlock& operator=(const BasicBlock&) = delete;

    BasicBlock(BasicBlock&&) = delete;
    BasicBlock& operator=(BasicBlock&&) = delete;

    void verify() const;

    Instruction& insertInstruction(iterator It, Opcodes Opcode, std::optional<SourceInfo> SrcInf = std::nullopt) {
        auto Instr = Instruction(Opcode, this, SrcInf);
        if (!Instructions.empty() && std::prev(Instructions.end())->isTerminal() &&
            Instr.isTerminal())
            throwVerifyError("Trying to add terminal instruction to basic block that already has it");

        return *Instructions.insert(It, std::move(Instr));
    }
    Instruction& addInstruction(Opcodes Opcode, std::optional<SourceInfo> SrcInf = std::nullopt) {
        return insertInstruction(Instructions.end(), Opcode, SrcInf);
    }

    Function* getParentFunction() const { return ParentFunction; }
    std::string_view getName() const { return Name; }
    std::optional<SourceInfo> getSrcInfo() const { return SrcInfo; }

    iterator       begin()        { return Instructions.begin(); }
    iterator       end()          { return Instructions.end(); }
    const_iterator begin() const  { return Instructions.cbegin(); }
    const_iterator end()   const  { return Instructions.cend(); }
    bool           empty()  const { return Instructions.empty(); }

    void addPredecessor(BasicBlock* BB) { Predecessors.insert(Predecessors.end(), BB); }
    const bb_storage& getPredecessors() const { return Predecessors; }

    void addSuccessor(BasicBlock* BB) { Successors.insert(Successors.end(), BB); }
    const bb_storage& getSuccessors() const { return Successors; }

private:
    instructions_storage Instructions;
    bb_storage Successors;
    bb_storage Predecessors;
    Function* ParentFunction;
    std::string Name;
    std::optional<SourceInfo> SrcInfo;

    void throwVerifyError(std::string error) const {
        // TODO: print basic block info
        throw Instruction::verify_error(error);
    }
};

} // Balance

#endif // IR_BASICBLOCK_H
