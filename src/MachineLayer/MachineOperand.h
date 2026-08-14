#ifndef MACHINE_OPERAND_H
#define MACHINE_OPERAND_H

#include "Register.h"

#include <cstdint>
#include <iostream>
#include <variant>

namespace Balance {

class MachineBB;
class MachineInst;
class MachineFunction;

// Global data: variables and arrays in .data and .rodata
class MachineGData {
public:
    MachineGData(std::string VarName, size_t ElemBytes, bool IsConst, std::vector<int64_t>&& InitVals) :
        Name(VarName), ElemSize(ElemBytes), Const(IsConst), Init(InitVals) {}

    std::string_view getName() const { return Name; }
    bool isConst() const { return Const; }
    const std::vector<int64_t>& getInit() const { return Init; }
    size_t getSize() const { return Init.size() * ElemSize; }

private:
    std::string Name;
    size_t ElemSize;
    bool Const;
    std::vector<int64_t> Init;
};

class MachineOperand {
    std::variant<Register, int64_t, MachineBB *, MachineFunction*, MachineGData*> Value;

    MachineInst *MI;

    bool IsDef = false;
    bool IsUse = false;

    std::string AsmString;

public:
    MachineOperand(MachineInst *MI = nullptr) : Value(Register(0)), MI(MI) {}
    MachineOperand(Register R, MachineInst *MI = nullptr) : Value(R), MI(MI) {}
    MachineOperand(int64_t Imm, MachineInst *MI = nullptr) : Value(Imm), MI(MI) {}
    MachineOperand(MachineBB *MBB, MachineInst *MI = nullptr) : Value(MBB), MI(MI) {}
    MachineOperand(MachineFunction *MF, MachineInst *MI = nullptr) : Value(MF), MI(MI) {}
    MachineOperand(MachineGData *Data, MachineInst *MI = nullptr) : Value(Data), MI(MI) {}

    bool isReg() const;
    bool isImm() const;
    bool isMBB() const;
    bool isFunc() const;
    bool isGData() const;

    Register getReg() const;
    uint64_t getImm() const;
    MachineBB *getMBB() const;
    MachineFunction *getFunc() const;
    MachineGData *getGData() const;

    Register setReg(Register NewReg);
    uint64_t setImm(uint64_t NewImm);
    MachineBB *setMBB(MachineBB *NewMBB);
    MachineFunction *setFunc(MachineFunction *NewFunc);
    MachineGData* setGData(MachineGData *Data);

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
#endif // MACHINE_OPERAND_H
