#ifndef REGISTER_H
#define REGISTER_H

#include "RISCV/RISCVRegisters.h"

#include <string>
#include <iostream>

namespace Balance {

// general register class
class Register {

public:
    enum class Type {
        Virtual,
        Physical
    };

private:
    int RegId;
    Type RegType = Type::Virtual;
    enum State { Define, Used, LastUsed } State;

public:
    Register(int RegId = -1) : RegId(RegId),
                               RegType(Type::Virtual) {}
    Register(RISCV::RISCVRegister Reg) : RegId(static_cast<unsigned>(Reg)),
                                         RegType(Type::Physical) {}

    Type getType() const;
    void setType(Type NewT);
    unsigned getId() const;
    std::string getAsmString() const;
    void print(std::ostream &OS) const;
    bool operator==(const Register &Reg2) const;
    bool operator!=(const Register &Reg2) const;
    bool operator<(const Register &Reg2) const;
    bool operator>(const Register &Reg2) const;
};

bool isReservedRegister(Register Reg);

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




#endif // REGISTER_H
