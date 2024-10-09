#include "trap.h"
#include <stdlib.h>

int main() {
    void *p = malloc(6400);
    nemu_assert(p);
    nemu_assert(p);
    return 0;
}