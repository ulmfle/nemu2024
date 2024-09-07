#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    DATA_TYPE res;
    if (op_src->type == OP_TYPE_REG) POP(REG(op_src->reg));
    else {
        POP(res);
        OPERAND_W(op_src, res);
    }
    print_asm_template1();
}

make_instr_helper(rm);
make_instr_helper(r);

#include "cpu/exec/template-end.h"
