#include "cpu/exec/template-start.h"

make_helper(concat(in_i2a_, SUFFIX)) {
    int len = decode_i_b(eip + 1);
    REG(R_EAX) = pio_read((ioaddr_t)op_src->val, DATA_BYTE);

    print_asm("in" str(SUFFIX) " %s,%%%s", op_src->str, REG_NAME(R_EAX));
    return len + 1;
}

make_helper(concat(in_d2a_, SUFFIX)) {
    REG(R_EAX) = pio_read((ioaddr_t)cpu.edx, DATA_BYTE);

    print_asm("in" str(SUFFIX) " %%dx,%%%s", REG_NAME(R_EAX));
    return 1;
}

#include "cpu/exec/template-end.h"