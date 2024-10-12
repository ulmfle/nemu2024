#include "cpu/exec/template-start.h"

#define instr push

#if DATA_BYTE == 1
make_helper(push_i_b) {
    int len = decode_i_b(eip + 1);
    cpu.esp -= 4;Log("%08x", (int32_t)(op_src->val));
    swaddr_write(cpu.esp, 4, SR_SS, (int32_t)(op_src->val));

    print_asm_template1();
    return len + 1;
}
#endif

#if DATA_BYTE != 1
static void do_execute() {
    PUSH(op_src->val);
    print_asm_template1();
}

make_instr_helper(i);
make_instr_helper(r);
make_instr_helper(rm);
#endif

#include "cpu/exec/template-end.h"