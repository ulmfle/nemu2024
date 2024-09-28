#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(int_b) {
	int len = decode_i_b(eip + 1);

	raise_intr(op_src->val);

	print_asm("int %s", op_src->str);
	return len + 1;
}