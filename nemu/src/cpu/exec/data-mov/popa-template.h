#include "cpu/exec/template-start.h"

#define instr popa

make_helper(concat(popa_, SUFFIX)) {
    DATA_TYPE nul;
    POP(REG(R_EDI));
    POP(REG(R_ESI));
    POP(REG(R_EBP));
    POP(nul);
    POP(REG(R_EBX));
    POP(REG(R_EDX));
    POP(REG(R_ECX));
    POP(REG(R_EAX));
    
    print_asm_template1();
    return 1;
}

#include "cpu/exec/template-end.h"