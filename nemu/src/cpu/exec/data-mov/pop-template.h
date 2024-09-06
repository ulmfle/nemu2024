#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    POP(REG(op_src->reg));
    print_asm_template1();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"
