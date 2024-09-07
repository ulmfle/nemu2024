#include "cpu/exec/template-start.h"

make_helper(concat(enter_, SUFFIX)) {
    int len = decode_i_w(eip + 1);
    uint16_t alloc = op_src->val;
    len += decode_i_b(eip + len + 1);
    int level = op_src->val & 31;

    PUSH(REG(R_EBP));

    uint32_t frame_ptr = REG(R_ESP);

    if (level) {
        int i = 1;
        for (; i < level; ++i) {
            PUSH(REG(R_EBP));
        }

        PUSH(frame_ptr);
    }

    PUSH((DATA_TYPE)alloc);

    return len + 1;
}

#include "cpu/exec/template-end.h"