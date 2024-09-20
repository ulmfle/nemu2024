#include "cpu/exec/template-start.h"

make_helper(concat(lgdt_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);

    cpu.gdtr.limit = swaddr_read(op_src->addr, 2);
    cpu.gdtr.LBA = swaddr_read(op_src->addr, 4) & (~0u >> (DATA_BYTE == 2 ? 8 : 0));

    return len + 1;
}

#include "cpu/exec/template-end.h"