#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_rel_,SUFFIX)) {
    int len = decode_si_l(eip + 1);

    PUSH(eip + len + 1);

    cpu.eip += op_src->simm & (~0u >> (DATA_BYTE == 2 ? 16 : 0));

    print_asm_template1();
    return len + 1;
}

make_helper(concat(call_rm_,SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);

    PUSH(eip + len + 1);

    if (op_src->type == OP_TYPE_REG) {
        cpu.eip = (REG(op_src->reg) & (~0u >> (DATA_BYTE == 2 ? 16 : 0))) - (len + 1);
    } else {
        cpu.eip = (MEM_R(SR_DS ,op_src->addr) & (~0u >> (DATA_BYTE == 2 ? 16 : 0))) - (len + 1);
    }

    print_asm_template1();
    return len + 1;
}

#include "cpu/exec/template-end.h"
