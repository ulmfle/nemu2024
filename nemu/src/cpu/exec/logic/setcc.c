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

#define instr seta
SETCC_COND(a, !(cpu.eflags.ZF | cpu.eflags.CF))
#undef instr

#define instr setae
SETCC_COND(ae, !cpu.eflags.CF)
#undef instr

#define instr setb
SETCC_COND(b, cpu.eflags.CF)
#undef instr

#define instr setbe
SETCC_COND(be, cpu.eflags.ZF | cpu.eflags.CF)
#undef instr

#define instr setc
SETCC_COND(c, cpu.eflags.CF)
#undef instr

#define instr setcxz
SETCC_COND(cxz, !REG(R_CX))
#undef instr

#define instr setecxz
SETCC_COND(ecxz, !REG(R_ECX))
#undef instr

#define instr sete
SETCC_COND(e, cpu.eflags.ZF)
#undef instr

#define instr setg
SETCC_COND(g, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setge
SETCC_COND(ge, !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setl
SETCC_COND(l, cpu.eflags.SF ^ cpu.eflags.OF)
#undef instr

#define instr setle
SETCC_COND(le, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setna
SETCC_COND(na, cpu.eflags.CF | cpu.eflags.ZF)
#undef instr

#define instr setnae
SETCC_COND(nae, cpu.eflags.CF)
#undef instr

#define instr setnb
SETCC_COND(nb, !cpu.eflags.CF)
#undef instr

#define instr setnbe
SETCC_COND(nbe, !cpu.eflags.CF & !cpu.eflags.CF)
#undef instr

#define instr setnc
SETCC_COND(nc, !cpu.eflags.CF)
#undef instr

#define instr setne
SETCC_COND(ne, !cpu.eflags.ZF)
#undef instr

#define instr setng
SETCC_COND(ng, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setnge
SETCC_COND(nge, cpu.eflags.SF ^ cpu.eflags.OF)
#undef instr

#define instr setnl
SETCC_COND(nl, !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setnle
SETCC_COND(nle, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr setno
SETCC_COND(no, !cpu.eflags.OF)
#undef instr

#define instr setnp
SETCC_COND(np, !cpu.eflags.PF)
#undef instr

#define instr setns
SETCC_COND(ns, !cpu.eflags.SF)
#undef instr

#define instr setnz
SETCC_COND(nz, !cpu.eflags.ZF)
#undef instr

#define instr seto
SETCC_COND(o, cpu.eflags.OF)
#undef instr

#define instr setp
SETCC_COND(p, cpu.eflags.PF)
#undef instr

#define instr setpe
SETCC_COND(pe, cpu.eflags.PF)
#undef instr

#define instr setpo
SETCC_COND(po, !cpu.eflags.PF)
#undef instr

#define instr sets
SETCC_COND(s, cpu.eflags.SF)
#undef instr

#define instr setz
SETCC_COND(z, cpu.eflags.ZF)
#undef instr

#include "cpu/exec/template-end.h"
#undef DATA_BYTE

