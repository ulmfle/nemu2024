#include "cpu/exec/template-start.h"

make_helper(concat(leave_, SUFFIX)) {
    REG(R_ESP) = REG(R_EBP);
    POP(REG(R_EBP));
    print_asm("leave");
    return 1;
}

#include "cpu/exec/template-end.h"