#ifndef RISCV_REGISTERS_H
#define RISCV_REGISTERS_H

#include "Utils/Utils.h"

#include <string_view>

using namespace Balance;
namespace RISCV {

// ai-generated
// This enum combines all RISC-V hardware registers, including integer (X) and floating-point (F) registers,
// into a single enumeration for unified access.
enum class RISCVRegister {
    // Hardware Name    ABI Name / Alias
    X0 = 0,             ZERO = 0,   // Hard-wired zero
    X1 = 1,             RA = 1,     // Return address
    X2 = 2,             SP = 2,     // Stack pointer
    X3 = 3,             GP = 3,     // Global pointer
    X4 = 4,             TP = 4,     // Thread pointer
    X5 = 5,             T0 = 5,     // Temporary 0
    X6 = 6,             T1 = 6,     // Temporary 1
    X7 = 7,             T2 = 7,     // Temporary 2
    X8 = 8,             S0 = 8,     // Saved register 0 / Frame Pointer (FP)
    FP = 8,                         // Frame Pointer alias
    X9 = 9,             S1 = 9,     // Saved register 1
    X10 = 10,           A0 = 10,    // Function argument 0 / Return value 0
    X11 = 11,           A1 = 11,    // Function argument 1 / Return value 1
    X12 = 12,           A2 = 12,    // Function argument 2
    X13 = 13,           A3 = 13,    // Function argument 3
    X14 = 14,           A4 = 14,    // Function argument 4
    X15 = 15,           A5 = 15,    // Function argument 5
    X16 = 16,           A6 = 16,    // Function argument 6
    X17 = 17,           A7 = 17,    // Function argument 7
    X18 = 18,           S2 = 18,    // Saved register 2
    X19 = 19,           S3 = 19,    // Saved register 3
    X20 = 20,           S4 = 20,    // Saved register 4
    X21 = 21,           S5 = 21,    // Saved register 5
    X22 = 22,           S6 = 22,    // Saved register 6
    X23 = 23,           S7 = 23,    // Saved register 7
    X24 = 24,           S8 = 24,    // Saved register 8
    X25 = 25,           S9 = 25,    // Saved register 9
    X26 = 26,           S10 = 26,   // Saved register 10
    X27 = 27,           S11 = 27,   // Saved register 11
    X28 = 28,           T3 = 28,    // Temporary 3
    X29 = 29,           T4 = 29,    // Temporary 4
    X30 = 30,           T5 = 30,    // Temporary 5
    X31 = 31,           T6 = 31,    // Temporary 6

    // Floating Point Registers
    F0 = 32,            FT0 = 32,   // FP Temporary 0
    F1 = 33,            FT1 = 33,   // FP Temporary 1
    F2 = 34,            FT2 = 34,   // FP Temporary 2
    F3 = 35,            FT3 = 35,   // FP Temporary 3
    F4 = 36,            FT4 = 36,   // FP Temporary 4
    F5 = 37,            FT5 = 37,   // FP Temporary 5
    F6 = 38,            FT6 = 38,   // FP Temporary 6
    F7 = 39,            FT7 = 39,   // FP Temporary 7
    F8 = 40,            FS0 = 40,   // FP Saved register 0
    F9 = 41,            FS1 = 41,   // FP Saved register 1
    F10 = 42,           FA0 = 42,   // FP Argument 0 / Return value 0
    F11 = 43,           FA1 = 43,   // FP Argument 1 / Return value 1
    F12 = 44,           FA2 = 44,   // FP Argument 2
    F13 = 45,           FA3 = 45,   // FP Argument 3
    F14 = 46,           FA4 = 46,   // FP Argument 4
    F15 = 47,           FA5 = 47,   // FP Argument 5
    F16 = 48,           FA6 = 48,   // FP Argument 6
    F17 = 49,           FA7 = 49,   // FP Argument 7
    F18 = 50,           FS2 = 50,   // FP Saved register 2
    F19 = 51,           FS3 = 51,   // FP Saved register 3
    F20 = 52,           FS4 = 52,   // FP Saved register 4
    F21 = 53,           FS5 = 53,   // FP Saved register 5
    F22 = 54,           FS6 = 54,   // FP Saved register 6
    F23 = 55,           FS7 = 55,   // FP Saved register 7
    F24 = 56,           FS8 = 56,   // FP Saved register 8
    F25 = 57,           FS9 = 57,   // FP Saved register 9
    F26 = 58,           FS10 = 58,  // FP Saved register 10
    F27 = 59,           FS11 = 59,  // FP Saved register 11
    F28 = 60,           FT8 = 60,   // FP Temporary 8
    F29 = 61,           FT9 = 61,   // FP Temporary 9
    F30 = 62,           FT10 = 62,  // FP Temporary 10
    F31 = 63,           FT11 = 63   // FP Temporary 11
};

// ai-generated
namespace {
struct RegisterInfo {
    const char *AsmName;
};

static const RegisterInfo RegisterDB[] = {
    // Integer Registers (ABI Names)
    {"zero"}, // X0
    {"ra"},   // X1
    {"sp"},   // X2
    {"gp"},   // X3
    {"tp"},   // X4
    {"t0"},   // X5
    {"t1"},   // X6
    {"t2"},   // X7
    {"s0"},   // X8
    {"s1"},   // X9
    {"a0"},   // X10
    {"a1"},   // X11
    {"a2"},   // X12
    {"a3"},   // X13
    {"a4"},   // X14
    {"a5"},   // X15
    {"a6"},   // X16
    {"a7"},   // X17
    {"s2"},   // X18
    {"s3"},   // X19
    {"s4"},   // X20
    {"s5"},   // X21
    {"s6"},   // X22
    {"s7"},   // X23
    {"s8"},   // X24
    {"s9"},   // X25
    {"s10"},  // X26
    {"s11"},  // X27
    {"t3"},   // X28
    {"t4"},   // X29
    {"t5"},   // X30
    {"t6"},   // X31

    // Floating-Point Registers (ABI Names)
    {"ft0"},  // F0
    {"ft1"},  // F1
    {"ft2"},  // F2
    {"ft3"},  // F3
    {"ft4"},  // F4
    {"ft5"},  // F5
    {"ft6"},  // F6
    {"ft7"},  // F7
    {"fs0"},  // F8
    {"fs1"},  // F9
    {"fa0"},  // F10
    {"fa1"},  // F11
    {"fa2"},  // F12
    {"fa3"},  // F13
    {"fa4"},  // F14
    {"fa5"},  // F15
    {"fa6"},  // F16
    {"fa7"},  // F17
    {"fs2"},  // F18
    {"fs3"},  // F19
    {"fs4"},  // F20
    {"fs5"},  // F21
    {"fs6"},  // F22
    {"fs7"},  // F23
    {"fs8"},  // F24
    {"fs9"},  // F25
    {"fs10"}, // F26
    {"fs11"}, // F27
    {"ft8"},  // F28
    {"ft9"},  // F29
    {"ft10"}, // F30
    {"ft11"}  // F31
};
} // anonymous namespace

inline std::string_view getRegAsmString(unsigned RegId) {
    if (RegId >= (sizeof(RegisterDB) / sizeof(RegisterDB[0]))) {
        unreachable("Invalid register ID");
    }
    return RegisterDB[RegId].AsmName;
}

inline bool isReservedRegister(RISCVRegister Reg) {
    switch (Reg) {
        case RISCVRegister::ZERO:
        case RISCVRegister::RA:
        case RISCVRegister::SP:
        case RISCVRegister::GP:
            return true;
        default:
            return false;
    }
}

} // namespace RISCV

#endif // RISCV_REGISTERS_H
