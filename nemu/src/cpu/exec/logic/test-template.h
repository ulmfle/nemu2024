#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
    uint32_t res = op_src->val & op_dest->val;
    cpu.eflags.CF = cpu.eflags.OF = 0;
    update_eflags_pf_zf_sf((DATA_TYPE_S)res);
    print_asm_template2();
}

make_instr_helper(i2rm);
make_instr_helper(i2r);
make_instr_helper(r2rm);

#include "cpu/exec/template-end.h"