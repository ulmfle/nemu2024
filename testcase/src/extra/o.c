#include "trap.h"

int main() {
    asm volatile ("mov $0x7fffffff,%eax");
    asm volatile ("sub $0x1,%eax");
    asm volatile ("seto %cl");
    asm volatile ("setc %bl");
    return 0;
}