#include "trap.h"
#include <stdlib.h>

int main() {
    void *p = malloc(100);
    nemu_assert(p);
    p = malloc(1000);
    nemu_assert(p);
    return 0;
}