#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
    DATA_TYPE res = op_dest->val - op_src->val;
    uint8_t dst = op_dest->val >> (8*DATA_BYTE-2);
    uint8_t src = op_dest->val >> (8*DATA_BYTE-2);
    update_eflags_pf_zf_sf(res);
    res >>= 8*DATA_BYTE - 1;
    cpu.eflags.CF = ~((dst & 1) ^ (src & 1)) ^ ~((dst & 1) ^ res);
    cpu.eflags.OF = ~((dst >> 1) ^ (src >> 1)) ^ ((dst >> 1) ^ res);
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