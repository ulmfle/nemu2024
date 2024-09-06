#include "cpu/exec/helper.h"

#define instr ret

make_helper(ret) {
    uint32_t addr = swaddr_read(cpu.esp, 4);
    cpu.esp += 4;
    cpu.eip = addr - 1;
    print_asm("ret");
    return 1;
}

make_helper(ret_l) {
    decode_i_l(eip + 1);
    uint32_t addr = swaddr_read(cpu.esp, 4);
    cpu.esp += 4;
    cpu.eip = addr - 1;
    cpu.esp += op_src->val;
    print_asm("ret %s", op_src->str);
    return 3;
}