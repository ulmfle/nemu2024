#include "trap.h"

int main() {
    asm volatile("push $0xf1");
    return 0;
}