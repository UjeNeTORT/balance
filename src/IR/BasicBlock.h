#ifndef IR_BASICBLOCK_H_
#define IR_BASICBLOCK_H_

#include "Instruction.h"
#include <list>
#include <memory>

namespace Balance {

class BasicBlock {
public:
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

    std::optional<SourceInfo> getSourceInfo() const { return SourceInfo; }

private:
    std::list<std::unique_ptr<Instruction>> Instructions;
    std::optional<SourceInfo> SourceInfo;
};

} // Balance

#endif // IR_BASICBLOCK_H_
