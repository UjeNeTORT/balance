#ifndef IR_PASSES_IRPASS_H
#define IR_PASSES_IRPASS_H

#include "IR/Function.h"
#include "Pass/Pass.h"

namespace Balance {

class IRPass : public Pass<Function> {
public:
    using Pass<Function>::Pass;
};

} // namespace Balance

#endif // IR_PASSES_IRPASS_H
