#ifndef IR_FUNCTION_H_
#define IR_FUNCTION_H_

#include "IR/BasicBlock.h"
#include "IR/Operand.h"

#include <iterator>
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

    void addArg(OpType Arg) { Args.push_back(Arg); }
    void setRetType(std::optional<OpType> Type) { RetType = Type; }
    void incFrameSize(size_t Increment) { FrameSize += Increment; }

    std::string_view getName() const { return Name; }
    const std::vector<OpType>& getArgs() const { return Args; }
    std::optional<OpType> getRetType() const { return RetType; }

    iterator addBasicBlock() {
        return BasicBlocks.insert(BasicBlocks.end(), {this,
                            Name + "_bb" + std::to_string(getNewBBId())});
    }

    Instruction& addInstruction(Opcodes Opcode,
                                        std::optional<SourceInfo> SrcInfo = std::nullopt) {
        iterator LastBB;
        if (BasicBlocks.empty()) {
            LastBB = addBasicBlock();
        } else {
            LastBB = std::prev(BasicBlocks.end());

            if (!LastBB->empty() && std::prev(LastBB->end())->isTerminal())
                LastBB = addBasicBlock();
        }
        return LastBB->addInstruction(Opcode, SrcInfo);
    }

    void verify() const {
        if (BasicBlocks.empty())
            return;

        if (entryBB() == nullptr ||
            entryBB()->begin()->getOpcode() != Opcodes::FUNC_DEF)
            throw Instruction::verify_error("Function's first basic block must begin with FUNC_DEF");

        for (const auto& BB: BasicBlocks)
            BB.verify();
    }

    BasicBlock* entryBB() {
        if (BasicBlocks.empty())
            return nullptr;

        return &*BasicBlocks.begin();
    }
    const BasicBlock* entryBB() const {
        if (BasicBlocks.empty())
            return nullptr;

        return &*BasicBlocks.begin();
    }

    size_t getNewBBId() { return BBCounter++; }

    VirtRegister getNewVirtReg(OpType Type, std::optional<BasicBlock*> DefBlock = std::nullopt) {
        return {Type, VirtRegCounter++, DefBlock};
    }
    template<size_t NUM>
    std::array<VirtRegister, NUM> getNewVirtRegs(OpType Type,
                                            std::optional<BasicBlock*> DefBlock = std::nullopt) {
        std::array<VirtRegister, NUM> Res;
        for (auto& Elem: Res)
            Elem = getNewVirtReg(Type, DefBlock);
        return Res;
    }

    VirtRegister getFrameVReg() const {
        return entryBB()->begin()->getDst()[0];
    }

    size_t getFrameSize() const {
        return FrameSize;
    }

    iterator       begin()       { return BasicBlocks.begin(); }
    iterator       end()         { return BasicBlocks.end(); }
    const_iterator begin() const { return BasicBlocks.cbegin(); }
    const_iterator end()   const { return BasicBlocks.cend(); }
private:
    std::string Name;
    std::vector<OpType> Args;
    std::optional<OpType> RetType;
    size_t FrameSize = 0;

    std::list<BasicBlock> BasicBlocks;
    size_t BBCounter = 0;
    int VirtRegCounter = 1;
};

}

#endif // IR_FUNCTION_H_
