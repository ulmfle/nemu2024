#include "trap.h"

int main() {
    asm volatile("push $0x7f");
    return 0;
}