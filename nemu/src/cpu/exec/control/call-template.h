#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_rel_,SUFFIX)) {
    uint32_t len = concat(decode_i_, SUFFIX)(eip + 1);
    cpu.eip += op_src->val;
    return len + 1;
}

#include "cpu/exec/template-end.h"
