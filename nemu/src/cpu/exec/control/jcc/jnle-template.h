#include "cpu/exec/template-start.h"

#define instr jnle

static void do_execute() {
    if (!cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF)) cpu.eip += (DATA_TYPE_S)op_src->val;
    print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h" 

#undef instr