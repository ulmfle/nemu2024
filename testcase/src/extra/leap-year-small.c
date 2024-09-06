#include "trap.h"

int is_leap_year(int n) {
	return (n % 4 == 0 && n % 100 != 0) || (n % 400 == 0);
}

int ans[] = {0, 0, 1};

#define NR_DATA (sizeof(ans) / sizeof(ans[0]))

int main() {
	//for(i = 0; i < NR_DATA; i ++) {
		nemu_assert(is_leap_year(0 + 1890) == ans[0]);
		nemu_assert(is_leap_year(1 + 1890) == ans[1]);
		nemu_assert(is_leap_year(2 + 1890) == ans[2]);
	//}

	//nemu_assert(i == NR_DATA);

	return 0;
}
