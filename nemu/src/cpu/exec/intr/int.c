#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(int_b) {
	decode_i_b(eip + 1);
	print_asm("int %s", op_src->str);
	cpu.eip += 2;
	raise_intr(op_src->val);

	return 0;
}