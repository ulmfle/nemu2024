#ifndef __INTR_H__
#define __INTR_H__

#include "nemu.h"
#include <setjmp.h>
extern jmp_buf jbuf;

typedef struct {
    uint32_t ptr;
	uint16_t sel;
} _lptr;

void raise_intr(uint8_t);
void load_desc(uint8_t, uint16_t);
void lnread64(lnaddr_t, void *);
void pop32(uint32_t *dst);
void push32(uint32_t *src);
void pop64(uint64_t *dst);
void push64(uint64_t *src);

#endif