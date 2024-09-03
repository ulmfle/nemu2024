#include "cpu/exec/helper.h"

#define instr ret

make_helper(ret) {
    uint32_t addr = swaddr_read(cpu.esp, 4);
    cpu.esp += 4;
    cpu.eip = addr - 1;
    print_asm("ret");
    return 1;
}