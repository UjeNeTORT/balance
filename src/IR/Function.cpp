#include "Function.h"

using namespace Balance;

void Function::verify() const {
    if (IsDecl) {
        assert(BasicBlocks.empty());
        return;
    }
    assert(!BasicBlocks.empty());

    if (entryBB() == nullptr ||
        entryBB()->begin()->getOpcode() != Opcodes::FUNC_DEF)
        throw Instruction::verify_error("Function's first basic block must begin with FUNC_DEF");

    if (entryBB()->getPredecessors().size() != 0)
        throw Instruction::verify_error("Functions's entry basic block must have no predecessors");

    for (const auto& BB: BasicBlocks)
        BB.verify();
}
