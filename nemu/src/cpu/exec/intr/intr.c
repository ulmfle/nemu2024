#include "nemu.h"
#include <setjmp.h>
extern jmp_buf jbuf;

void raise_intr(uint8_t NO) {
    Log("INTED");
    longjmp(jbuf,1);
}