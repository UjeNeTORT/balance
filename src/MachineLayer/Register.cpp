#include "Register.h"

using namespace Balance;

using Type = Register::Type;

Type Register::getType() const { return RegType; }
void Register::setType(Type NewT) { RegType = NewT; }
bool Register::isVirtual() const { return RegType == Type::Virtual; }
bool Register::isPhysical() const { return RegType == Type::Physical; }
unsigned Register::getId() const { return RegId; }
std::string Register::getAsmString() const {
    std::string S;
    switch (RegType) {
    case Type::Virtual:
        S += std::string("%") + std::to_string(RegId);
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
void Register::print(std::ostream &OS) const {
    OS << getAsmString();
}

bool Register::operator==(const Register &Reg2) const { return RegType == Reg2.RegType && RegId == Reg2.RegId; }
bool Register::operator!=(const Register &Reg2) const { return !(*this == Reg2); }

bool Register::operator<(const Balance::Register &R2) const {
    if (RegType != R2.RegType) return RegType < R2.RegType;
    return RegId < R2.RegId;
}

bool Register::operator>(const Balance::Register &R2) const {
    return R2 < *this;
}
