#include "cpu/exec/helper.h"

make_helper(push_i_b) {
    int len = decode_i_b(eip + 1);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, (int32_t)(op_src->val), cpu.ss.sel.index);
    return len + 1;
}

#define DATA_BYTE 2
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

make_helper_v(push_i);
make_helper_v(push_r);
make_helper_v(push_rm);