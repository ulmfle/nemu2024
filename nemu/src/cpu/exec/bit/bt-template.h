#include "cpu/exec/template-start.h"

#define instr bt

static void do_execute() {
    cpu.eflags.CF = op_dest->val & (1 << op_src->val);

    print_asm_template2();
}

make_instr_helper(r2rm);
make_instr_helper(i2rm);

#include "cpu/exec/template-end.h"