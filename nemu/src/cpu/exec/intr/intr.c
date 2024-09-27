#include "intr.h"

void push32(uint32_t *src) {
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, SR_SS, *src);
}

void pop32(uint32_t *dst) {
    *dst = swaddr_read(cpu.esp, 4, SR_SS);
    cpu.esp += 4;
}

void push64(uint64_t *src) {
    push32(&(((uint32_t *)src)[0]));
    push32(&(((uint32_t *)src)[1]));
}

void pop64(uint64_t *dst) {
    pop32(&(((uint32_t *)dst)[1]));
    pop32(&(((uint32_t *)dst)[0]));
}

void raise_intr(uint8_t NO) {
    uint64_t buf;
    GateDesc gd;
    buf = lnread64(cpu.idtr.LBA + sizeof(GateDesc)*NO);
    memcpy((void *)&gd, (void *)&buf, sizeof(GateDesc));

    longjmp(jbuf,1);
}