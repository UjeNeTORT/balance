#include "StackFrameAlloc.h"

#include "MachineLayer/MIROpcodes.h"
#include "RISCV/RISCVRegisters.h"

#include <cassert>
#include <cstdint>

using namespace Balance;

bool StackFrameAlloc::run(MachineFunction &MF) {
    size_t FrameSize = (MF.getFrameSize() + 15) & ~15; //< align up to 16 multiplier
    if (FrameSize == 0)
        return false;

    using RVOp = RISCVOpcode;
    using RVReg = RISCV::RISCVRegister;

    MF.entryBB()->insertMI(MF.entryBB()->begin(),
                           MachineInst(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP)
                                                  .addImm(-static_cast<int64_t>(FrameSize))); // TODO: check overflow

    for (auto& MBB: MF) {
        for (auto MI = MBB.begin(); MI != MBB.end(); MI++) {
            if (MI->getOpcode() == RVOp::RET)
                MBB.insertMI(MI, MachineInst(RVOp::ADDI).addReg(RVReg::SP).addReg(RVReg::SP)
                                                        .addImm(static_cast<int64_t>(FrameSize))); // TODO: check overflow
        }
    }

    return true;
}
