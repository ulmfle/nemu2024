#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
    uint32_t res = op_src->val & op_dest->val;
    cpu.eflags.CF = cpu.eflags.OF = 0;
    update_eflags_pf_zf_sf((DATA_TYPE_S)res);
    print_asm_template2();
}

make_instr_helper(i2rm);
make_instr_helper(r2rm);

make_helper(concat(test_i2eax_, SUFFIX)) {
    int len = concat(decode_i_, SUFFIX)(eip + 1);
    uint32_t res = op_src->val & REG(0);
    cpu.eflags.CF = cpu.eflags.OF = 0;
    update_eflags_pf_zf_sf((DATA_TYPE_S)res);
    print_asm(str(instr) str(SUFFIX) " %s,%s", op_src->str, REG_NAME(0));
    return len + 1;
}

#include "cpu/exec/template-end.h"