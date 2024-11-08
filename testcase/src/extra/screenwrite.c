#include "trap.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define vmem 0xa0000
#define SCR_SIZE (320 * 200)
#define CTR_COL 320
#define CTR_ROW 200

inline void setpix(int x, int y, uint8_t color) {
    memcpy(vmem+CTR_ROW*y+x, &color, 1);
}

int main() {
    setpix(3, 3, 0xff);
    HIT_GOOD_TRAP;
    return 0;
}