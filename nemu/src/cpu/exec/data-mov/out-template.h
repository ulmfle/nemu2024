#include "cpu/exec/template-start.h"

make_helper(concat(out_a2i_, SUFFIX)) {
    int len = decode_i_b(eip + 1);
    pio_write((uint16_t)op_src->val, DATA_BYTE, REG(R_EAX));

    print_asm("out" str(SUFFIX) " %%%s, %s", REG_NAME(R_EAX), op_src->str);
    return len + 1;
}

make_helper(concat(out_a2d_, SUFFIX)) {
    pio_write((uint16_t)cpu.edx, DATA_BYTE, REG(R_EAX));

    print_asm("out" str(SUFFIX) " %%%s,%%dx", REG_NAME(R_EAX));
    return 1;
}

#include "cpu/exec/template-end.h"