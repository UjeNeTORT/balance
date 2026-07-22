#ifndef IR_BASICBLOCK_H_
#define IR_BASICBLOCK_H_

#include "Instruction.h"

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

    BasicBlock(Function* Parent, std::string Name)
        : ParentFunction(Parent)
        , Name(Name)
    {}
    BasicBlock(Function* Parent, std::string Name, SourceInfo SrcInf)
        : ParentFunction(Parent)
        , Name(Name)
        , SrcInfo(SrcInf)
    {}

    void verify() const {
        bool IsTerminal = false;
        for (auto& Instr: Instructions) {
            Instr.verify();
            if (IsTerminal)
                throw Instruction::verify_error("Terminal instruction not in the end of basic block");
            IsTerminal = Instr.isTerminal();
        }
        if (!IsTerminal)
            throw Instruction::verify_error("No terminal instruction in the end of basic block");
    }

    iterator insertInstruction(iterator It, Opcodes Opcode, std::optional<SourceInfo> SrcInf = std::nullopt) {
        auto Instr = Instruction(Opcode, this, SrcInf);
        if (!Instructions.empty() && std::prev(Instructions.end())->isTerminal() &&
            Instr.isTerminal())
            throwVerifyError("Trying to add terminal instruction to basic block that already has it");

        return Instructions.insert(It, std::move(Instr));

    }
    iterator addInstruction(Opcodes Opcode, std::optional<SourceInfo> SrcInf = std::nullopt) {
        return insertInstruction(Instructions.end(), Opcode, SrcInf);
    }

    Function* getParentFunction() const { return ParentFunction; }
    std::string_view getName() const { return Name; }
    std::optional<SourceInfo> getSrcInfo() const { return SrcInfo; }

    iterator       begin()        { return Instructions.begin(); }
    iterator       end()          { return Instructions.end(); }
    const_iterator cbegin() const { return Instructions.cbegin(); }
    const_iterator cend()   const { return Instructions.cend(); }
    bool           empty()  const { return Instructions.empty(); }

    bb_iterator       predecessorsBegin()        { return Predecessors.begin(); }
    bb_iterator       predecessorsEnd()          { return Predecessors.end(); }
    bb_const_iterator predecessorsCBegin() const { return Predecessors.cbegin(); }
    bb_const_iterator predecessorsCEnd()   const { return Predecessors.cend(); }

    std::list<BasicBlock*> getSuccessors() const {
        std::list<BasicBlock*> Successors;
        if (Instructions.empty())
            return Successors;

        for (auto& Succ: std::prev(Instructions.end())->getBrDstBB())
            Successors.push_back(Succ);
        return Successors;
    }

private:
    instructions_storage Instructions;
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

#endif // IR_BASICBLOCK_H_
