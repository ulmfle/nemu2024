#include "trap.h"

int main() {
    asm volatile("push $0x1f");
    return 0;
}