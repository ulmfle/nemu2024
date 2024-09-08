#include "trap.h"
int main() {
    asm volatile ("mov $0x7fffffff,%eax");
    asm volatile ("add $0x1,%eax");
    asm volatile ("seto %cl");
    return 0;
}