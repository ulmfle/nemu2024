#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

long long ans[] = {0x8000000000000000LL,0x7fffffffffffffffLL,-1};

int main() {
    nemu_assert(add(ans[0],ans[1]) == ans[2]);
    return 0;
}