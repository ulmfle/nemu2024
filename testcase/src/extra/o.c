#include "trap.h"

int main() {
    asm volatile ("mov $80000000,%eax");
    asm volatile ("sub $0x1,%eax");
    asm volatile ("seto %cl");
    asm volatile ("setc %bl");
    return 0;
}