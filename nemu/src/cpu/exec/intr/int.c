#include "cpu/exec/helper.h"

void raise_intr(uint8_t);

make_helper(int_b) {
	decode_i_b(eip + 1);
	raise_intr(op_src->val);

	print_asm("int %s", op_src->str);
	return 2;
}