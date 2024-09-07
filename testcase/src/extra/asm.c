#include "trap.h"

int main() {
    asm volatile ("mov $0x100, %eax");
    asm volatile ("cmp $0x100, %eax");
    return 0;
}