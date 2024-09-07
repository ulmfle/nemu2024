#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "cpu/exec/template-start.h"

make_helper(push_i_b) {
    int len = decode_si_b(eip + 1);
    PUSH_NBYTE((uint32_t)op_src->simm, 4);
    print_asm("pushb %s",op_src->str);
    return len + 1;
}

#include "cpu/exec/template-end.h"
#undef DATA_BYTE


#define DATA_BYTE 2
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

make_helper_v(push_i);
make_helper_v(push_r);
make_helper_v(push_rm);