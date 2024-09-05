#include "cpu/exec/template-start.h"

#define instr jcxz

static void do_execute() {
    if (!reg_w(R_CX)) cpu.eip += op_src->val;
    print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"

#undef instr