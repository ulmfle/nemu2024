#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    POP(op_src->reg);
    print_asm_template1();
}

make_instr_helper(r);
make_instr_helper(rm);

#include "cpu/exec/template-end.h"
