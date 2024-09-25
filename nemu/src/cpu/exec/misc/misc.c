#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(int_b) {
	decode_i_b(eip + 1);
	void do_int3();
	do_int3();
	print_asm("int %s", op_src->str);

	return 2;
}

make_helper(stc) {
	cpu.eflags.CF = 1;
	print_asm("stc");
	return 1;
}

make_helper(sti) {
	cpu.eflags.IF = 1;
	print_asm("sti");
	return 1;
}

make_helper(std) {
	cpu.eflags.DF = 1;
	print_asm("std");
	return 1;
}

make_helper(cld) {
	cpu.eflags.DF = 0;
	print_asm("cld");
	return 1;
}

make_helper(clc) {
	cpu.eflags.CF = 0;
	print_asm("clc");
	return 1;
}

make_helper(cli) {
	cpu.eflags.IF = 0;
	print_asm("cli");
	return 1;
}

make_helper(clts) {
	cpu.cr0.task_switched = 0;
	print_asm("clts");
	return 1;
}

make_helper(cmc) {
	cpu.eflags.CF = !cpu.eflags.CF;
	print_asm("cmc");
	return 1;
}