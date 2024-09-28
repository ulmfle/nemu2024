#include "cpu/exec/template-start.h"

#define instr pusha

make_helper(concat(pusha_, SUFFIX)) {
    DATA_TYPE temp = REG(R_ESP);
    PUSH(REG(R_EAX));
    PUSH(REG(R_ECX));
    PUSH(REG(R_EDX));
    PUSH(REG(R_EBX));
    PUSH(temp);
    PUSH(REG(R_EBP));
    PUSH(REG(R_ESI));
    PUSH(REG(R_EDI));

    print_asm("pusha" str(SUFFIX));
    return 1;
}

#include "cpu/exec/template-end.h"