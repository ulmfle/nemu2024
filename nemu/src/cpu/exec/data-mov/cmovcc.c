#include "cpu/exec/helper.h"

#define CMOVCC_COND(name, cond) \
make_helper(concat5(cmov, name, _, rm2r_, SUFFIX)) {\
    int len = concat(decode_rm2r_, SUFFIX)(eip + 1);\
    if (cond) OPERAND_W(op_dest, op_src->val);\
    print_asm(str(concat(cmov, name)) " %s,%s", op_src->str, REG_NAME(op_dest->reg));\
    return len + 1;\
}

#define DATA_BYTE 2
#include "cmovcc-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmovcc-template.h"
#undef DATA_BYTE

make_helper_v(cmova_rm2r);
make_helper_v(cmovae_rm2r);
make_helper_v(cmovb_rm2r);
make_helper_v(cmovbe_rm2r);
make_helper_v(cmovc_rm2r);
make_helper_v(cmovcxz_rm2r);
make_helper_v(cmovecxz_rm2r);
make_helper_v(cmove_rm2r);
make_helper_v(cmovg_rm2r);
make_helper_v(cmovge_rm2r);
make_helper_v(cmovl_rm2r);
make_helper_v(cmovle_rm2r);
make_helper_v(cmovna_rm2r);
make_helper_v(cmovnae_rm2r);
make_helper_v(cmovnb_rm2r);
make_helper_v(cmovnbe_rm2r);
make_helper_v(cmovnc_rm2r);
make_helper_v(cmovne_rm2r);
make_helper_v(cmovng_rm2r);
make_helper_v(cmovnge_rm2r);
make_helper_v(cmovnl_rm2r);
make_helper_v(cmovnle_rm2r);
make_helper_v(cmovno_rm2r);
make_helper_v(cmovnp_rm2r);
make_helper_v(cmovns_rm2r);
make_helper_v(cmovnz_rm2r);
make_helper_v(cmovo_rm2r);
make_helper_v(cmovp_rm2r);
make_helper_v(cmovpe_rm2r);
make_helper_v(cmovpo_rm2r);
make_helper_v(cmovs_rm2r);
make_helper_v(cmovz_rm2r);