#ifndef IR_IR_H_
#define IR_IR_H_

#include "BasicBlock.h"
#include "Instruction.h"
#include "Function.h"

namespace Balance {

class IR {
public:
    using FunctionStorage = std::vector<Function>;
    using iterator = FunctionStorage::iterator;
    using const_iterator = FunctionStorage::const_iterator;

    void verify() const {
        for (const auto& Func: Functions)
            Func.verify();
    }

    iterator       begin()       { return Functions.begin(); }
    iterator       end()         { return Functions.end(); }
    const_iterator begin() const { return Functions.cbegin(); }
    const_iterator end()   const { return Functions.cend(); }
private:
    FunctionStorage Functions;
};

} // Balance

#endif // IR_IR_H_
