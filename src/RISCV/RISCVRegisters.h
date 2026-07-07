#ifndef RISCV_REGISTERS_H
#define RISCV_REGISTERS_H

namespace RISCV {

enum RegistersX {
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
    X31 = 31,           T6 = 31     // Temporary 6
};

enum RegistersF {
    // Hardware Name    ABI Name / Alias
    F0 = 0,             FT0 = 0,    // FP Temporary 0
    F1 = 1,             FT1 = 1,    // FP Temporary 1
    F2 = 2,             FT2 = 2,    // FP Temporary 2
    F3 = 3,             FT3 = 3,    // FP Temporary 3
    F4 = 4,             FT4 = 4,    // FP Temporary 4
    F5 = 5,             FT5 = 5,    // FP Temporary 5
    F6 = 6,             FT6 = 6,    // FP Temporary 6
    F7 = 7,             FT7 = 7,    // FP Temporary 7
    F8 = 8,             FS0 = 8,    // FP Saved register 0
    F9 = 9,             FS1 = 9,    // FP Saved register 1
    F10 = 10,           FA0 = 10,   // FP Argument 0 / Return value 0
    F11 = 11,           FA1 = 11,   // FP Argument 1 / Return value 1
    F12 = 12,           FA2 = 12,   // FP Argument 2
    F13 = 13,           FA3 = 13,   // FP Argument 3
    F14 = 14,           FA4 = 14,   // FP Argument 4
    F15 = 15,           FA5 = 15,   // FP Argument 5
    F16 = 16,           FA6 = 16,   // FP Argument 6
    F17 = 17,           FA7 = 17,   // FP Argument 7
    F18 = 18,           FS2 = 18,   // FP Saved register 2
    F19 = 19,           FS3 = 19,   // FP Saved register 3
    F20 = 20,           FS4 = 20,   // FP Saved register 4
    F21 = 21,           FS5 = 21,   // FP Saved register 5
    F22 = 22,           FS6 = 22,   // FP Saved register 6
    F23 = 23,           FS7 = 23,   // FP Saved register 7
    F24 = 24,           FS8 = 24,   // FP Saved register 8
    F25 = 25,           FS9 = 25,   // FP Saved register 9
    F26 = 26,           FS10 = 26,  // FP Saved register 10
    F27 = 27,           FS11 = 27,  // FP Saved register 11
    F28 = 28,           FT8 = 28,   // FP Temporary 8
    F29 = 29,           FT9 = 29,   // FP Temporary 9
    F30 = 30,           FT10 = 30,  // FP Temporary 10
    F31 = 31,           FT11 = 31   // FP Temporary 11
};

} // namespace RISCV

#endif // RISCV_REGISTERS_H
