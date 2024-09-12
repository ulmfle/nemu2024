#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

long long test_data[] = {0, 1, 2, 0x7fffffffffffffffLL, 0x8000000000000000LL, 0x8000000000000001LL, 0xfffffffffffffffeLL, 0xffffffffffffffffLL};
long long ans[] = {0LL, 0x1LL, 0x2LL, 0x7fffffffffffffffLL, 0x8000000000000000LL, 0x8000000000000001LL, 0xfffffffffffffffeLL, 0xffffffffffffffffLL, 0x1LL, 0x2LL, 0x3LL, 0x8000000000000000LL, 0x8000000000000001LL, 0x8000000000000002LL, 0xffffffffffffffffLL, 0LL, 0x2LL, 0x3LL, 0x4LL, 0x8000000000000001LL, 0x8000000000000002LL, 0x8000000000000003LL, 0LL, 0x1LL, 0x7fffffffffffffffLL, 0x8000000000000000LL, 0x8000000000000001LL, 0xfffffffffffffffeLL, 0xffffffffffffffffLL, 0LL, 0x7ffffffffffffffdLL, 0x7ffffffffffffffeLL, 0x8000000000000000LL, 0x8000000000000001LL, 0x8000000000000002LL, 0xffffffffffffffffLL, 0LL, 0x1LL, 0x7ffffffffffffffeLL, 0x7fffffffffffffffLL, 0x8000000000000001LL, 0x8000000000000002LL, 0x8000000000000003LL, 0LL, 0x1LL, 0x2LL, 0x7fffffffffffffffLL, 0x8000000000000000LL, 0xfffffffffffffffeLL, 0xffffffffffffffffLL, 0LL, 0x7ffffffffffffffdLL, 0x7ffffffffffffffeLL, 0x7fffffffffffffffLL, 0xfffffffffffffffcLL, 0xfffffffffffffffdLL, 0xffffffffffffffffLL, 0LL, 0x1LL, 0x7ffffffffffffffeLL, 0x7fffffffffffffffLL, 0x8000000000000000LL, 0xfffffffffffffffdLL, 0xfffffffffffffffeLL};

int main() {
    nemu_assert(add(ans[0],ans[1]) == ans[1]);
    return 0;
}