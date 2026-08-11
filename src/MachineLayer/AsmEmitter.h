#ifndef MACHINE_LAYER_ASM_EMITTER_H_
#define MACHINE_LAYER_ASM_EMITTER_H_

#include "MIR.h"
#include "MachineFunction.h"
#include "MachineBB.h"
#include "MachineInst.h"

#include <ostream>

namespace Balance {

class AsmEmitter {
public:
    AsmEmitter(const MIR& MachineIR, std::ostream& OutputStream)
        : Mir(MachineIR)
        , OS(OutputStream)
    {}

    void emit() &&;
private:
    const MIR& Mir;
    std::ostream& OS;

    void emitFunction(const MachineFunction& Func);
    void emitBasicBlock(const MachineBB& BB);
    void emitInstr(const MachineInst& Instr);
    void emitInstrOp(const MachineOperand& Op);
    void emitBBLabel(const MachineBB& BB);
    void emitGlobalVar(const MachineGData& Var);
};

} // namespace Balance

#endif //< MACHINE_LAYER_ASM_EMITTER_H_
