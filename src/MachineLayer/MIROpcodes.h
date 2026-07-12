#ifndef MIR_OPCODES_H
#define MIR_OPCODES_H

#include <string_view>

namespace Balance {

// ai-generated
enum RISCVOpcode {
    //===------------------------------------------------------------------===//
    // 0. Target-Independent Generic MIR Opcodes (Crucial for GlobalISel-style design)
    //===------------------------------------------------------------------===//
    G_ADD, G_SUB, G_MUL, G_AND, G_OR, G_XOR, G_SHL, G_LSHR, G_ASHR,
    G_LOAD, G_STORE, G_CONSTANT, G_FRAME_INDEX, G_BR, G_BRCOND,

    //===------------------------------------------------------------------===//
    // 1. RV64I Base Integer Extension (64-bit Base)
    //===------------------------------------------------------------------===//
    // Computational Instructions (32-bit & 64-bit Word variants)
    LUI, AUIPC,
    ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,
    ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
    ADDIW, SLLIW, SRLIW, SRAIW,
    ADDW, SUBW, SLLW, SRLW, SRAW,

    // Loads and Stores
    LB, LH, LW, LD, LBU, LHU, LWU,
    SB, SH, SW, SD,

    // Control Flow
    JAL, JALR,
    BEQ, BNE, BLT, BGE, BLTU, BGEU,

    // System & Sync (Often mapped to base I)
    ECALL, EBREAK, FENCE, FENCE_I,

    // Zicsr (Standard CSR instructions)
    CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI,

    //===------------------------------------------------------------------===//
    // 2. "M" Standard Extension (Integer Multiplication and Division)
    //===------------------------------------------------------------------===//
    MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
    MULW, DIVW, DIVUW, REMW, REMUW,

    //===------------------------------------------------------------------===//
    // 3. "A" Standard Extension (Atomic Memory Operations)
    //===------------------------------------------------------------------===//
    LR_W, SC_W, AMOSWAP_W, AMOADD_W, AMOXOR_W, AMOAND_W, AMOOR_W, AMOMIN_W, AMOMAX_W, AMOMINU_W, AMOMAXU_W,
    LR_D, SC_D, AMOSWAP_D, AMOADD_D, AMOXOR_D, AMOAND_D, AMOOR_D, AMOMIN_D, AMOMAX_D, AMOMINU_D, AMOMAXU_D,

    //===------------------------------------------------------------------===//
    // 4. "F" Standard Extension (Single-Precision Floating-Point)
    //===------------------------------------------------------------------===//
    FLW, FSW,
    FMADD_S, FMSUB_S, FNMSUB_S, FNMADD_S,
    FADD_S, FSUB_S, FMUL_S, FDIV_S, FSQRT_S,
    FSGNJ_S, FSGNJN_S, FSGNJX_S,
    FMIN_S, FMAX_S,
    FCVT_W_S, FCVT_WU_S, FCVT_S_W, FCVT_S_WU,
    FMV_X_W, FMV_W_X,
    FEQ_S, FLT_S, FLE_S,
    FCLASS_S,
    FCVT_L_S, FCVT_LU_S, FCVT_S_L, FCVT_S_LU, // RV64F Specific

    //===------------------------------------------------------------------===//
    // 5. "D" Standard Extension (Double-Precision Floating-Point)
    //===------------------------------------------------------------------===//
    FLD, FSD,
    FMADD_D, FMSUB_D, FNMSUB_D, FNMADD_D,
    FADD_D, FSUB_D, FMUL_D, FDIV_D, FSQRT_D,
    FSGNJ_D, FSGNJN_D, FSGNJX_D,
    FMIN_D, FMAX_D,
    FCVT_S_D, FCVT_D_S,
    FEQ_D, FLT_D, FLE_D,
    FCLASS_D,
    FCVT_W_D, FCVT_WU_D, FCVT_D_W, FCVT_D_WU,
    FCVT_L_D, FCVT_LU_D, FMV_X_D, FCVT_D_L, FCVT_D_LU, FMV_D_X, // RV64D Specific

    //===------------------------------------------------------------------===//
    // 6. "C" Standard Extension (16-bit Compressed Instructions)
    //===------------------------------------------------------------------===//
    C_ADDI4SPN, C_FLD, C_LW, C_LD, C_FSD, C_SW, C_SD,
    C_NOP, C_ADDI, C_ADDIW, C_LI, C_ADDI16SP, C_LUI,
    C_SRLI, C_SRAI, C_ANDI, C_SUB, C_XOR, C_OR, C_AND, C_SUBW, C_ADDW,
    C_J, C_BEQZ, C_BNEZ, C_SLLI, C_FLDSP, C_LWSP, C_LDSP,
    C_JR, C_MV, C_EBREAK, C_JALR, C_ADD, C_FSDSP, C_SWSP, C_SDSP,

    //===------------------------------------------------------------------===//
    // MIR-specific pseudo-insts
    //===------------------------------------------------------------------===//
    PHI,
};

class MachineInst;
bool isControlTransferInst(const MachineInst &MI);
unsigned getNumDefs(RISCVOpcode Opcode);

std::string_view getInstNameByOpcode(RISCVOpcode Opcode);

} // namespace Balance

#endif // MIR_OPCODES_H
