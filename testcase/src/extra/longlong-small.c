#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

int main() {
    long long ans = 3;
    nemu_assert(add(1,2) == ans);
    return 0;
}