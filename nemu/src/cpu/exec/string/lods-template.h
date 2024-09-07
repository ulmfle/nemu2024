#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
    REG(R_EAX) = REG(R_ESI);
    REG(R_ESI) += (cpu.eflags.DF ? -DATA_BYTE : DATA_BYTE);

    print_asm("lods" str(SUFFIX) " %%ds:(%%esi),%%eax");
    return 1;
}

#include "cpu/exec/template-end.h"
