#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_rel_,SUFFIX)) {
    int len = decode_si_l(eip + 1);
    cpu.eip += op_src->simm + DATA_BYTE + 1;
    return len + 1;
}

#include "cpu/exec/template-end.h"
