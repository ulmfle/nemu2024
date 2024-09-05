#include "cpu/exec/template-start.h"

#define instr jnb

static void do_execute() {
    if (!cpu.eflags.CF) cpu.eip += (DATA_TYPE_S)op_src->val;
    print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"

#undef instr