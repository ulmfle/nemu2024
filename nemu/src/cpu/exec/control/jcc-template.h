#include "cpu/exec/template-start.h"

#define instr ja
JCC_COND(a, !(cpu.eflags.ZF | cpu.eflags.CF))
#undef instr

#define instr jae
JCC_COND(ae, !cpu.eflags.CF)
#undef instr

#define instr jb
JCC_COND(b, cpu.eflags.CF)
#undef instr

#define instr jbe
JCC_COND(be, cpu.eflags.ZF | cpu.eflags.CF)
#undef instr

#define instr jc
JCC_COND(c, cpu.eflags.CF)
#undef instr

#define instr jcxz
JCC_COND(cxz, !REG(R_CX))
#undef instr

#define instr jecxz
JCC_COND(ecxz, !REG(R_ECX))
#undef instr

#define instr je
JCC_COND(e, cpu.eflags.ZF)
#undef instr

#define instr jg
JCC_COND(g, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jge
JCC_COND(ge, !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jl
JCC_COND(l, cpu.eflags.SF ^ cpu.eflags.OF)
#undef instr

#define instr jle
JCC_COND(le, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jna
JCC_COND(na, cpu.eflags.CF | cpu.eflags.ZF)
#undef instr

#define instr jnae
JCC_COND(nae, cpu.eflags.CF)
#undef instr

#define instr jnb
JCC_COND(nb, !cpu.eflags.CF)
#undef instr

#define instr jnbe
JCC_COND(nbe, !cpu.eflags.CF & !cpu.eflags.CF)
#undef instr

#define instr jnc
JCC_COND(nc, !cpu.eflags.CF)
#undef instr

#define instr jne
JCC_COND(ne, !cpu.eflags.ZF)
#undef instr

#define instr jng
JCC_COND(ng, cpu.eflags.ZF | (cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jnge
JCC_COND(nge, cpu.eflags.SF ^ cpu.eflags.OF)
#undef instr

#define instr jnl
JCC_COND(nl, !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jnle
JCC_COND(nle, !cpu.eflags.ZF & !(cpu.eflags.SF ^ cpu.eflags.OF))
#undef instr

#define instr jno
JCC_COND(no, !cpu.eflags.OF)
#undef instr

#define instr jnp
JCC_COND(np, !cpu.eflags.PF)
#undef instr

#define instr jns
JCC_COND(ns, !cpu.eflags.SF)
#undef instr

#define instr jnz
JCC_COND(nz, !cpu.eflags.ZF)
#undef instr

#define instr jo
JCC_COND(o, cpu.eflags.OF)
#undef instr

#define instr jp
JCC_COND(p, cpu.eflags.PF)
#undef instr

#define instr jpe
JCC_COND(pe, cpu.eflags.PF)
#undef instr

#define instr jpo
JCC_COND(po, !cpu.eflags.PF)
#undef instr

#define instr js
JCC_COND(s, cpu.eflags.SF)
#undef instr

#define instr jz
JCC_COND(z, cpu.eflags.ZF)
#undef instr

#include "cpu/exec/template-end.h"