#include "trap.h"
#include <stdlib.h>

int main() {
    nemu_assert(malloc(100));
    return 0;
}