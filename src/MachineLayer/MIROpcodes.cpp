// ai-generated
#include "MIROpcodes.h"
#include "MachineInst.h"

#include "Utils/Utils.h"

#include <string_view>

namespace Balance {

bool isControlTransferInst(const MachineInst &MI) {
    switch (MI.getOpcode()) {
    case G_BR:
    case G_BRCOND:
    case JAL:
    case JALR:
    case BEQ:
    case BNE:
    case BLT:
    case BGE:
    case BLTU:
    case BGEU:
        return true;
    default:
        return false;
    }
}

// ai-generated
std::string_view getInstNameByOpcode(RISCVOpcode Opcode) {
    switch (Opcode) {
    case G_ADD:         return "G_ADD";
    case G_SUB:         return "G_SUB";
    case G_MUL:         return "G_MUL";
    case G_AND:         return "G_AND";
    case G_OR:          return "G_OR";
    case G_XOR:         return "G_XOR";
    case G_SHL:         return "G_SHL";
    case G_LSHR:        return "G_LSHR";
    case G_ASHR:        return "G_ASHR";
    case G_LOAD:        return "G_LOAD";
    case G_STORE:       return "G_STORE";
    case G_CONSTANT:    return "G_CONSTANT";
    case G_FRAME_INDEX: return "G_FRAME_INDEX";
    case G_BR:          return "G_BR";
    case G_BRCOND:      return "G_BRCOND";
    case LUI:           return "LUI";
    case AUIPC:         return "AUIPC";
    case ADDI:          return "ADDI";
    case SLTI:          return "SLTI";
    case SLTIU:         return "SLTIU";
    case XORI:          return "XORI";
    case ORI:           return "ORI";
    case ANDI:          return "ANDI";
    case SLLI:          return "SLLI";
    case SRLI:          return "SRLI";
    case SRAI:          return "SRAI";
    case ADD:           return "ADD";
    case SUB:           return "SUB";
    case SLL:           return "SLL";
    case SLT:           return "SLT";
    case SLTU:          return "SLTU";
    case XOR:           return "XOR";
    case SRL:           return "SRL";
    case SRA:           return "SRA";
    case OR:            return "OR";
    case AND:           return "AND";
    case ADDIW:         return "ADDIW";
    case SLLIW:         return "SLLIW";
    case SRLIW:         return "SRLIW";
    case SRAIW:         return "SRAIW";
    case ADDW:          return "ADDW";
    case SUBW:          return "SUBW";
    case SLLW:          return "SLLW";
    case SRLW:          return "SRLW";
    case SRAW:          return "SRAW";
    case LB:            return "LB";
    case LH:            return "LH";
    case LW:            return "LW";
    case LD:            return "LD";
    case LBU:           return "LBU";
    case LHU:           return "LHU";
    case LWU:           return "LWU";
    case SB:            return "SB";
    case SH:            return "SH";
    case SW:            return "SW";
    case SD:            return "SD";
    case JAL:           return "JAL";
    case JALR:          return "JALR";
    case BEQ:           return "BEQ";
    case BNE:           return "BNE";
    case BLT:           return "BLT";
    case BGE:           return "BGE";
    case BLTU:          return "BLTU";
    case BGEU:          return "BGEU";
    case ECALL:         return "ECALL";
    case EBREAK:        return "EBREAK";
    case FENCE:         return "FENCE";
    case FENCE_I:       return "FENCE_I";
    case CSRRW:         return "CSRRW";
    case CSRRS:         return "CSRRS";
    case CSRRC:         return "CSRRC";
    case CSRRWI:        return "CSRRWI";
    case CSRRSI:        return "CSRRSI";
    case CSRRCI:        return "CSRRCI";
    case MUL:           return "MUL";
    case MULH:          return "MULH";
    case MULHSU:        return "MULHSU";
    case MULHU:         return "MULHU";
    case DIV:           return "DIV";
    case DIVU:          return "DIVU";
    case REM:           return "REM";
    case REMU:          return "REMU";
    case MULW:          return "MULW";
    case DIVW:          return "DIVW";
    case DIVUW:         return "DIVUW";
    case REMW:          return "REMW";
    case REMUW:         return "REMUW";
    case LR_W:          return "LR_W";
    case SC_W:          return "SC_W";
    case AMOSWAP_W:     return "AMOSWAP_W";
    case AMOADD_W:      return "AMOADD_W";
    case AMOXOR_W:      return "AMOXOR_W";
    case AMOAND_W:      return "AMOAND_W";
    case AMOOR_W:       return "AMOOR_W";
    case AMOMIN_W:      return "AMOMIN_W";
    case AMOMAX_W:      return "AMOMAX_W";
    case AMOMINU_W:     return "AMOMINU_W";
    case AMOMAXU_W:     return "AMOMAXU_W";
    case LR_D:          return "LR_D";
    case SC_D:          return "SC_D";
    case AMOSWAP_D:     return "AMOSWAP_D";
    case AMOADD_D:      return "AMOADD_D";
    case AMOXOR_D:      return "AMOXOR_D";
    case AMOAND_D:      return "AMOAND_D";
    case AMOOR_D:       return "AMOOR_D";
    case AMOMIN_D:      return "AMOMIN_D";
    case AMOMAX_D:      return "AMOMAX_D";
    case AMOMINU_D:     return "AMOMINU_D";
    case AMOMAXU_D:     return "AMOMAXU_D";
    case FLW:           return "FLW";
    case FSW:           return "FSW";
    case FMADD_S:       return "FMADD_S";
    case FMSUB_S:       return "FMSUB_S";
    case FNMSUB_S:      return "FNMSUB_S";
    case FNMADD_S:      return "FNMADD_S";
    case FADD_S:        return "FADD_S";
    case FSUB_S:        return "FSUB_S";
    case FMUL_S:        return "FMUL_S";
    case FDIV_S:        return "FDIV_S";
    case FSQRT_S:       return "FSQRT_S";
    case FSGNJ_S:       return "FSGNJ_S";
    case FSGNJN_S:      return "FSGNJN_S";
    case FSGNJX_S:      return "FSGNJX_S";
    case FMIN_S:        return "FMIN_S";
    case FMAX_S:        return "FMAX_S";
    case FCVT_W_S:      return "FCVT_W_S";
    case FCVT_WU_S:     return "FCVT_WU_S";
    case FCVT_S_W:      return "FCVT_S_W";
    case FCVT_S_WU:     return "FCVT_S_WU";
    case FMV_X_W:       return "FMV_X_W";
    case FMV_W_X:       return "FMV_W_X";
    case FEQ_S:         return "FEQ_S";
    case FLT_S:         return "FLT_S";
    case FLE_S:         return "FLE_S";
    case FCLASS_S:      return "FCLASS_S";
    case FCVT_L_S:      return "FCVT_L_S";
    case FCVT_LU_S:     return "FCVT_LU_S";
    case FCVT_S_L:      return "FCVT_S_L";
    case FCVT_S_LU:     return "FCVT_S_LU";
    case FLD:           return "FLD";
    case FSD:           return "FSD";
    case FMADD_D:       return "FMADD_D";
    case FMSUB_D:       return "FMSUB_D";
    case FNMSUB_D:      return "FNMSUB_D";
    case FNMADD_D:      return "FNMADD_D";
    case FADD_D:        return "FADD_D";
    case FSUB_D:        return "FSUB_D";
    case FMUL_D:        return "FMUL_D";
    case FDIV_D:        return "FDIV_D";
    case FSQRT_D:       return "FSQRT_D";
    case FSGNJ_D:       return "FSGNJ_D";
    case FSGNJN_D:      return "FSGNJN_D";
    case FSGNJX_D:      return "FSGNJX_D";
    case FMIN_D:        return "FMIN_D";
    case FMAX_D:        return "FMAX_D";
    case FCVT_S_D:      return "FCVT_S_D";
    case FCVT_D_S:      return "FCVT_D_S";
    case FEQ_D:         return "FEQ_D";
    case FLT_D:         return "FLT_D";
    case FLE_D:         return "FLE_D";
    case FCLASS_D:      return "FCLASS_D";
    case FCVT_W_D:      return "FCVT_W_D";
    case FCVT_WU_D:     return "FCVT_WU_D";
    case FCVT_D_W:      return "FCVT_D_W";
    case FCVT_D_WU:     return "FCVT_D_WU";
    case FCVT_L_D:      return "FCVT_L_D";
    case FCVT_LU_D:     return "FCVT_LU_D";
    case FMV_X_D:       return "FMV_X_D";
    case FCVT_D_L:      return "FCVT_D_L";
    case FCVT_D_LU:     return "FCVT_D_LU";
    case FMV_D_X:       return "FMV_D_X";
    case C_ADDI4SPN:    return "C_ADDI4SPN";
    case C_FLD:         return "C_FLD";
    case C_LW:          return "C_LW";
    case C_LD:          return "C_LD";
    case C_FSD:         return "C_FSD";
    case C_SW:          return "C_SW";
    case C_SD:          return "C_SD";
    case C_NOP:         return "C_NOP";
    case C_ADDI:        return "C_ADDI";
    case C_ADDIW:       return "C_ADDIW";
    case C_LI:          return "C_LI";
    case C_ADDI16SP:    return "C_ADDI16SP";
    case C_LUI:         return "C_LUI";
    case C_SRLI:        return "C_SRLI";
    case C_SRAI:        return "C_SRAI";
    case C_ANDI:        return "C_ANDI";
    case C_SUB:         return "C_SUB";
    case C_XOR:         return "C_XOR";
    case C_OR:          return "C_OR";
    case C_AND:         return "C_AND";
    case C_SUBW:        return "C_SUBW";
    case C_ADDW:        return "C_ADDW";
    case C_J:           return "C_J";
    case C_BEQZ:        return "C_BEQZ";
    case C_BNEZ:        return "C_BNEZ";
    case C_SLLI:        return "C_SLLI";
    case C_FLDSP:       return "C_FLDSP";
    case C_LWSP:        return "C_LWSP";
    case C_LDSP:        return "C_LDSP";
    case C_JR:          return "C_JR";
    case C_MV:          return "C_MV";
    case C_EBREAK:      return "C_EBREAK";
    case C_JALR:        return "C_JALR";
    case C_ADD:         return "C_ADD";
    case C_FSDSP:       return "C_FSDSP";
    case C_SWSP:        return "C_SWSP";
    case C_SDSP:        return "C_SDSP";
    }
    unreachable("Unknown RISCVOpcode");
    return "";
}

} // namespace Balance
