#include "device/i8259.h"
#include "intr.h"
#include "cpu/exec/helper.h"

make_helper(hlt) {
    if (cpu.INTR & cpu.eflags.IF) {
        uint32_t intr_NO = i8259_query_intr();
        i8259_ack_intr();
        raise_intr(intr_NO);
    }
    return 0;       //loop when no == -1
}