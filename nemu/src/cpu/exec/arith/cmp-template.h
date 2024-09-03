#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    uint32_t res = op_dest->val - op_src->val;
    update_eflags_pf_zf_sf(res);
    if ((op_dest->val >> 31) == (op_src->val >> 31) &&
        (op_src->val >> 31) != (res >> 31)) cpu.eflags.OF = 1;
    

}

make_instr_helper(i2r);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);

#include "cpu/exec/template-end.h"