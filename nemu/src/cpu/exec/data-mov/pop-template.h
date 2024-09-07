#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    if (op_src->type == OP_TYPE_REG) POP(REG(op_src->reg));
    else POP(REG(op_src->addr));
    print_asm_template1();
}

make_instr_helper(rm);
make_instr_helper(r);

#include "cpu/exec/template-end.h"
