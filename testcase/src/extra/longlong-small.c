#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

int main() {
    nemu_assert(add(0x8000000000000000LL,0x7fffffffffffffffLL) == -1);
    return 0;
}