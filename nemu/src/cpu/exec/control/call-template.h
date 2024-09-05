#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_rel_,SUFFIX)) {
    int len = decode_si_l(eip + 1);

    PUSH_NBYTE(eip + len + 1, 4);

    cpu.eip += op_src->simm & ~0u >> (DATA_BYTE == 2 ? 16 : 0);

    print_asm_template1();
    return len + 1;
}

#include "cpu/exec/template-end.h"
