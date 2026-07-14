#include "Register.h"

using namespace Balance;

using Type = Register::Type;


Type Register::getType() const { return RegType; }
void Register::setType(Type NewT) { RegType = NewT; }
unsigned Register::getId() const { return RegId; }
std::string Register::getAsmString() const {
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
void Register::print(std::ostream &OS) const {
    OS << getAsmString();
}

bool Register::operator==(const Register &Reg2) const { return RegId == Reg2.RegId; }
bool Register::operator!=(const Register &Reg2) const { return !(*this == Reg2); }
