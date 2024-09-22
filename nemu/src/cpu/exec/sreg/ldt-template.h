#include "cpu/exec/template-start.h"

#define instr lgdt

make_helper(concat(lgdt_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);

    cpu.gdtr.limit = swaddr_read(op_src->addr, 2, SR_DS);
    cpu.gdtr.LBA = swaddr_read(op_src->addr + 2, 4, SR_DS) & (~0u >> (DATA_BYTE == 2 ? 8 : 0));

    print_asm_template1();
    return len + 1;
}

#undef instr

#include "cpu/exec/template-end.h"