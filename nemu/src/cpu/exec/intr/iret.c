#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(iret) {
    _lptr lptr;
    pop64((uint64_t *)&lptr);
    cpu.eip = lptr.ptr - 1;
    load_desc(SR_CS, lptr.sel);
    pop32(&cpu.eflags.val);

	print_asm("iret");
	return 1;
}