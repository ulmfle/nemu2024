#include "trap.h"

int NONAME(int v) {
    return v;
}

int main() {
    int a = NONAME(1);
    int b = NONAME(2);
    nemu_assert(a==1);
    nemu_assert(b==1);
    int c = a+b;
    nemu_assert(c == a+b);
    return 0;
}