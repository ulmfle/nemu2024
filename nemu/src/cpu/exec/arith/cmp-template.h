#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    DATA_TYPE result = op_dest->val - op_src->val;

    update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = ((op_dest->val >> (8*DATA_BYTE - 2)) & 1) != ((result >> (8*DATA_BYTE - 2)) & 1);
	cpu.eflags.OF = MSB((op_dest->val ^ op_src->val) & (op_dest->val ^ result));
    printf("result : %u, src = %u; reg = %d, dest = %u\n", result, op_dest->val, op_dest->reg, op_src->val);
    print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm);
#endif

make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);

#if DATA_BYTE == 1
make_helper(cmp_i2al_b) {
    DATA_TYPE result = REG(0) - op_src->val;

    update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = ((op_dest->val >> (8*DATA_BYTE - 2)) & 1) != ((result >> (8*DATA_BYTE - 2)) & 1);
	cpu.eflags.OF = MSB((op_dest->val ^ op_src->val) & (op_dest->val ^ result));
    printf("result : %u, src = %u; SPECREG, dest = %u\n", result, op_dest->val, op_src->val);
    print_asm_template2();
    return 2;
}
#else
make_helper(concat(cmp_i2eax_, SUFFIX)) {
    DATA_TYPE result = REG(0) - op_src->val;

    update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = ((op_dest->val >> (8*DATA_BYTE - 2)) & 1) != ((result >> (8*DATA_BYTE - 2)) & 1);
	cpu.eflags.OF = MSB((op_dest->val ^ op_src->val) & (op_dest->val ^ result));
    print_asm_template2();
    return 2;
}

#endif

#include "cpu/exec/template-end.h"