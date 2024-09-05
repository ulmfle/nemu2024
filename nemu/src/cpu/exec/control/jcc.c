#include "cpu/exec/helper.h"

#define JCC_COND(name, cond) \
make_helper(concat5(j, name, _, i_, SUFFIX)) {\
    int len = concat(decode_i_, SUFFIX)(eip + 1);\
    if (cond) cpu.eip += (DATA_TYPE_S)op_src->val;\
    print_asm_template1();\
    return len + 1;\
}

#define DATA_BYTE 1
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "jcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "jcc-template.h"
#undef DATA_BYTE

make_helper_v(ja_i);
make_helper_v(jae_i);
make_helper_v(jb_i);
make_helper_v(jbe_i);
make_helper_v(jc_i);
make_helper_v(jcxz_i);
make_helper_v(jecxz_i);
make_helper_v(je_i);
make_helper_v(jg_i);
make_helper_v(jge_i);
make_helper_v(jl_i);
make_helper_v(jle_i);
make_helper_v(jna_i);
make_helper_v(jnae_i);
make_helper_v(jnb_i);
make_helper_v(jnbe_i);
make_helper_v(jnc_i);
make_helper_v(jne_i);
make_helper_v(jng_i);
make_helper_v(jnge_i);
make_helper_v(jnl_i);
make_helper_v(jnle_i);
make_helper_v(jno_i);
make_helper_v(jnp_i);
make_helper_v(jns_i);
make_helper_v(jnz_i);
make_helper_v(jo_i);
make_helper_v(jp_i);
make_helper_v(jpe_i);
make_helper_v(jpo_i);
make_helper_v(js_i);
make_helper_v(jz_i);