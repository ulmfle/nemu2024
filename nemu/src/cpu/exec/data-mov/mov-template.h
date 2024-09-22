#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(SR_DS, addr, REG(R_EAX));

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(SR_DS, addr);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE == 2

make_helper(mov_rm2sr) {
	int len = decode_rm2r_w(eip + 1);
	sreg *_sr = &cpu.sr[op_dest->reg];
	_sr->sel.val = op_src->val;
	_sr->hid_desc.seg_present = 1;
	_sr->hid_desc.lo_32 = lnaddr_read(cpu.gdtr.LBA + _sr->sel.index * sizeof(descriptor), 4);
	print_asm("mov" str(SUFFIX) " %s,%%%s", op_src->str, regsr[op_dest->reg]);
	return len + 1;
}

make_helper(mov_sr2rm) {
	int len = decode_r2rm_w(eip + 1);
	OPERAND_W(op_dest, cpu.sr[op_src->reg].sel.val);
	print_asm("mov" str(SUFFIX) " %s,%%%s", regsr[op_src->reg], op_dest->str);
	return len + 1;
}

#endif

#if DATA_BYTE == 4

make_helper(mov_cr02r) {
    int len = decode_rm_l(eip + 1);
    OPERAND_W(op_src, cpu.cr0.val);

    print_asm("mov" str(SUFFIX) " %%%s,%%%s", "CR0", REG_NAME(op_src->reg));
    return len + 1;
}

make_helper(mov_r2cr0) {
    int len = decode_rm_l(eip + 1);
	cpu.cr0.val = op_src->val;

    print_asm("mov" str(SUFFIX) " %%%s,%%%s", REG_NAME(R_EAX), "CR0");
    return len + 1;
}

#endif

#include "cpu/exec/template-end.h"
