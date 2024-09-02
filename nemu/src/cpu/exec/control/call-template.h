#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.eip += op_src->val;
    print_asm_template2();
}

make_helper(concat(call_rel_,SUFFIX)) {
    
}

#include "cpu/exec/template-end.h"
