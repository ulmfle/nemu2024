#include "trap.h"

int sum;
int u(int x);
int d(int x);

int u(int x) {
    if (!x) return 0;
    return u(x-1) + d(x-1) + 1;
}

int d(int x) {
    if (!x) return 0;
    return u(x-1) + d(x-1) + 1;
}

int main() {
    int idx;
    for (idx = 1; idx <= 9; ++idx)
        sum += d(idx);
    //nemu_assert(sum == 1023);
    return 0;
}