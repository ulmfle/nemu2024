#include "trap.h"

int main() {
    asm ("mov $0x100, %eax");
    return 0;
}