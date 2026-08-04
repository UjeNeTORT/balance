#ifndef MIRBUILDER_H_
#define MIRBUILDER_H_

#include "IR/BasicBlock.h"
#include "IR/IR.h"
#include "MachineLayer/MIR.h"
#include "MachineLayer/MachineBB.h"

#include <map>

namespace Balance {

class MIRBuilder {
public:
    MIRBuilder(IR&& IntermRepr)
        : IntermRepr(std::move(IntermRepr))
    {}

    MIR build() &&;

private:
    IR IntermRepr;
    MIR MachineIR;

    std::map<Function*, MachineFunction*> FuncRegistry;

    void buildFunction(IR::iterator IRIt, MIR::iterator MIRIt);
    void buildBasicBlock(BasicBlock* IRBlock, MachineBB* MIRBlock,
                         std::map<const BasicBlock*, MachineBB*>& BBRegistry);
};

} // Balance

#endif // MIRBUILDER_H_
