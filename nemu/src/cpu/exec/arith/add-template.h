#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
    DATA_TYPE res = op_src->val + op_dest->val;
    OPERAND_W(op_dest, res);
    update_eflags_pf_zf_sf(res);
    cpu.eflags.CF = res > op_dest->val;
	//cpu.eflags.CF = ((op_dest->val >> (8*DATA_BYTE - 2)) & 1) != ((res >> (8*DATA_BYTE - 2)) & 1);
	cpu.eflags.OF = MSB((op_dest->val ^ op_src->val) & (op_dest->val ^ res));

    print_asm_template2();
}


#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm);
#endif

make_instr_helper(i2r);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);


#include "cpu/exec/template-end.h"