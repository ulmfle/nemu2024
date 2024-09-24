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
	load_desc(op_dest->sreg, op_src->val);

	print_asm("mov" str(SUFFIX) " %s,%%%s", op_src->str, regsr[op_dest->reg]);
	return len + 1;
}

make_helper(mov_sr2rm) {
	int len = decode_r2rm_w(eip + 1);
	OPERAND_W(op_dest, cpu.sr[op_src->sreg].sel.val);
	print_asm("mov" str(SUFFIX) " %s,%%%s", regsr[op_src->sreg], op_dest->str);
	return len + 1;
}

#endif

#if DATA_BYTE == 4

make_helper(mov_cr2r) {
    int len = decode_rm2r_l(eip + 1);
	switch (op_src->reg) {
	case R_EAX:
		OPERAND_W(op_dest, cpu.cr0.val);
		break;
	
	case R_EBX:
		OPERAND_W(op_dest, cpu.cr3.val);
		break;

	default:
		assert(0);
	}

    print_asm("mov" str(SUFFIX) " cr%d,%%%s", op_src->reg, REG_NAME(op_dest->reg));
    return len + 1;
}

make_helper(mov_r2cr) {
    int len = decode_rm2r_l(eip + 1);
	switch (op_dest->reg) {
		case R_EAX:
			cpu.cr0.val = op_src->val;
			break;
		
		case R_EBX:
			cpu.cr3.val = op_src->val;
			break;

		default:
			assert(0);
	}

    print_asm("mov" str(SUFFIX) " %%%s,cr%d", REG_NAME(op_src->reg), op_dest->reg);
    return len + 1;
}

#endif

#include "cpu/exec/template-end.h"
