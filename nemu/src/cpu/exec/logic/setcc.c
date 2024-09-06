#include "cpu/exec/helper.h"

#define SETCC_COND(name, cond)  \
make_helper(concat(set, name)) {\
    int len = decode_rm_b(eip + 1); \
    OPERAND_W(op_src, (cond));\
    print_asm(str(concat(set, name)) " %s", op_src->str);\
    return len + 1; \
}                               \

#define DATA_BYTE 1
#include "cpu/exec/template-start.h"

SETCC_COND(a, !(cpu.eflags.ZF | cpu.eflags.CF))
SETCC_COND(ae, !cpu.eflags.CF)
SETCC_COND(b, cpu.eflags.CF)
SETCC_COND(be, cpu.eflags.ZF | cpu.eflags.CF)
SETCC_COND(c, cpu.eflags.CF)
SETCC_COND(cxz, !REG(R_CX))
SETCC_COND(ecxz, !REG(R_ECX))
SETCC_COND(e, cpu.eflags.ZF)
SETCC_COND(g, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(ge, !(cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(l, cpu.eflags.SF ^ cpu.eflags.OF)
SETCC_COND(le, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(na, cpu.eflags.CF | cpu.eflags.ZF)
SETCC_COND(nae, cpu.eflags.CF)
SETCC_COND(nb, !cpu.eflags.CF)
SETCC_COND(nbe, !cpu.eflags.CF & !cpu.eflags.CF)
SETCC_COND(nc, !cpu.eflags.CF)
SETCC_COND(ne, !cpu.eflags.ZF)
SETCC_COND(ng, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(nge, cpu.eflags.SF ^ cpu.eflags.OF)
SETCC_COND(nl, !(cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(nle, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
SETCC_COND(no, !cpu.eflags.OF)
SETCC_COND(np, !cpu.eflags.PF)
SETCC_COND(ns, !cpu.eflags.SF)
SETCC_COND(nz, !cpu.eflags.ZF)
SETCC_COND(o, cpu.eflags.OF)
SETCC_COND(p, cpu.eflags.PF)
SETCC_COND(pe, cpu.eflags.PF)
SETCC_COND(po, !cpu.eflags.PF)
SETCC_COND(s, cpu.eflags.SF)
SETCC_COND(z, cpu.eflags.ZF)

#include "cpu/exec/template-end.h"
#undef DATA_BYTE

