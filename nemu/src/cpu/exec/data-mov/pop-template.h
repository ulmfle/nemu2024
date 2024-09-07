#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    POP(REG(op_src->reg));
    print_asm_template1();
}

make_helper(concat(pop_rm_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);
    DATA_TYPE r;
    POP(r);
    OPERAND_W(op_src, r);
    print_asm_template1();
    return len + 1;
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"
