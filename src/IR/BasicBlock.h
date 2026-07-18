#ifndef IR_BASICBLOCK_H_
#define IR_BASICBLOCK_H_

#include "Instruction.h"

#include <iterator>
#include <list>
#include <memory>
#include <string>

namespace Balance {

class Function;

class BasicBlock {
public:
    using instructions_storage = std::list<std::unique_ptr<Instruction>>;
    using iterator = instructions_storage::iterator;
    using const_iterator = instructions_storage::const_iterator;

    using bb_storage = std::list<BasicBlock*>;
    using bb_iterator = bb_storage::iterator;
    using bb_const_iterator = bb_storage::const_iterator;

    BasicBlock(Function* Parent)
        : ParentFunction(Parent)
    {}
    BasicBlock(Function* Parent, SourceInfo SrcInf)
        : ParentFunction(Parent)
        , SrcInfo(SrcInf)
    {}

    void verify() const {
        bool IsTerminal = false;
        for (auto& Instr: Instructions) {
            Instr->verify();
            if (IsTerminal)
                throw Instruction::verify_error("Terminal instruction not in the end of basic block");
            IsTerminal = Instr->isTerminal();
        }
        if (!IsTerminal)
            throw Instruction::verify_error("No terminal instruction in the end of basic block");
    }

    iterator addInstruction(Opcodes Opcode, std::optional<SourceInfo> SrcInf = std::nullopt) {
        auto Instr = std::make_unique<Instruction>(Opcode, this, SrcInf);
        if (!Instructions.empty() && std::prev(Instructions.end())->get()->isTerminal() &&
            Instr->isTerminal())
            throwVerifyError("Trying to add terminal instruction to basic block that already has it");

        return Instructions.insert(Instructions.end(), std::move(Instr));
    }

    Function* getParentFunction() const { return ParentFunction; }
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

private:
    instructions_storage Instructions;
    bb_storage Predecessors;
    Function* ParentFunction;
    std::optional<SourceInfo> SrcInfo;

    void throwVerifyError(std::string error) const {
        // TODO: print basic block info
        throw Instruction::verify_error(error);
    }
};

} // Balance

#endif // IR_BASICBLOCK_H_
