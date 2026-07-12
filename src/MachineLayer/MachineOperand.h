#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "RISCV/RISCVRegisters.h"
#include "Utils/Utils.h"

#include <cstdint>
#include <iostream>
#include <variant>

namespace Balance {

class MachineBB;
class MachineInst;

// general register class
class Register {

public:
    enum class Type {
        Virtual,
        Physical
    };

private:
    unsigned RegId;
    Type RegType = Type::Virtual;
    enum State { Define, Used, LastUsed } State;

public:
    Register(unsigned RegId) : RegId(RegId),
                               RegType(Type::Virtual) {}
    Register(RISCV::RISCVRegister Reg) : RegId(static_cast<unsigned>(Reg)),
                                         RegType(Type::Physical) {}

    Type getType() const { return RegType; }
    void setType(Type NewT) { RegType = NewT; }
    unsigned getId() const { return RegId; }
    std::string getAsmString() const {
        std::string S;
        switch (RegType) {
        case Type::Virtual:
            S += std::string("%VReg") + std::to_string(RegId);
            break;
        case Type::Physical:
            S += std::string(RISCV::getRegAsmString(RegId));
            break;
        default:
            unreachable("Unexpected register type");
            break;
        }

        return S;
    }
    void print(std::ostream &OS) const {
        OS << getAsmString();
    }

    bool operator==(const Register &Reg2) const { return RegId == Reg2.RegId; }
    bool operator!=(const Register &Reg2) const { return !(*this == Reg2); }
};

bool isReservedRegister(Register Reg);

class MachineOperand {
    std::variant<Register, uint64_t, MachineBB *> Value;

    MachineInst *MI;

    bool IsDef = false;
    bool IsUse = false;

    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : Value(Register(0)), MI(MI) {}
    MachineOperand(Register R, MachineInst *MI = nullptr) : Value(R), MI(MI) {}
    MachineOperand(uint64_t Imm, MachineInst *MI = nullptr) : Value(Imm), MI(MI) {}
    MachineOperand(MachineBB *MBB, MachineInst *MI = nullptr) : Value(MBB), MI(MI) {}

    bool isReg() const;
    bool isImm() const;
    bool isMBB() const;

    Register getReg() const;
    uint64_t getImm() const;
    MachineBB *getMBB() const;

    Register setReg(Register NewReg);
    uint64_t setImm(uint64_t NewImm);
    MachineBB *setMBB(MachineBB *NewMBB);

    MachineInst *getMI() const;
    void setMI(MachineInst *NewMI);

    bool isDef() const;
    bool isUse() const;

    void setIsDef(bool NewIsDef = true);
    void setIsUse(bool NewIsUse = true);

    std::string getAsmString() const;
    void print(std::ostream &OS) const;

    bool operator==(const MachineOperand &MO2) const;
    bool operator!=(const MachineOperand &MO2) const;
};


std::ostream &operator<<(std::ostream &OS, const MachineOperand &MO);
std::ostream &operator<<(std::ostream &OS, const Register &Reg);

} // namespace Balance
namespace std {
template<> struct hash<Balance::Register> {
    size_t operator()(const Balance::Register &R) const noexcept {
        size_t h1 = hash<unsigned>{}(R.getId());
        size_t h2 = hash<unsigned>{}(static_cast<unsigned>(R.getType()));
        return h1 ^ (h2 << 1);
    }
};
} // namespace std


#endif // MACHINE_OPERAND_H
