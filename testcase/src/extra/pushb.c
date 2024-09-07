#include "trap.h"

int main() {
    asm volatile("push $0x7f");
    asm volatile("pop %eax");
    return 0;
}