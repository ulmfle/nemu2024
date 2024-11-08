#include "trap.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define vmem_addr 0xa0000
#define SCR_SIZE (320 * 200)
#define CTR_COL 320
#define CTR_ROW 200

uint8_t (*vmem)[CTR_COL] = (void *)vmem_addr;

inline void setpix(int x, int y, uint8_t color) {
    vmem[y-1][x-1] = color;
}

int main() {
    setpix(3, 3, 0xff);
    setpix(4, 4, 0xff);
    setpix(5, 4, 0xff);
    setpix(5, 3, 0xff);
    setpix(6, 3, 0xff);
    setpix(7, 3, 0xff);
    setpix(8, 3, 0xff);
    setpix(9, 3, 0xff);
    HIT_GOOD_TRAP;
    return 0;
}