#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(iret) {
    _lptr lptr;
    pop64((uint64_t *)&lptr);
    load_desc(SR_CS, lptr.sel);
    cpu.eip = seg_translate(lptr.ptr, SR_CS) - 1;
    pop32(&cpu.eflags.val);

	print_asm("iret");
	return 1;
}