#include "trap.h"
int main() {
    asm volatile ("mov $0xffffffff,%eax");
    asm volatile ("add $0x1,%eax");
    asm volatile ("seto %cl");
    asm volatile ("setc %bl");
    return 0;
}