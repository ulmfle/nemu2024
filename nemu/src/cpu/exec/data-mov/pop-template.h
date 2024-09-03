#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    REG(op_src->reg) = MEM_R(REG(R_ESP));
    REG(R_ESP) += DATA_BYTE;
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"
