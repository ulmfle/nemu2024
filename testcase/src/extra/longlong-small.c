#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

int main() {
    nemu_assert(add(1,2) == 3LL);
    return 0;
}