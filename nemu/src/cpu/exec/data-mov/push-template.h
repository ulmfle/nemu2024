#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    PUSH(op_src->val);
    print_asm_template1();
}

make_instr_helper(r);
make_instr_helper(i);

make_helper(concat(push_rm_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);
    PUSH(MEM_R(op_src->addr));

    print_asm_template1();
    return len + 1;
}

#include "cpu/exec/template-end.h"