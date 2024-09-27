#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(int_b) {
	int len = decode_i_b(eip + 1);
    int no = op_src->val;
	uint64_t lptr;
	GateDesc gd;

	push32((uint32_t *)&cpu.eflags);
	push64(&lptr);
	
    lnread64(cpu.idtr.LBA + sizeof(GateDesc)*no, &gd);

	raise_intr(no);

	print_asm("int %s", op_src->str);
	return len + 1;
}