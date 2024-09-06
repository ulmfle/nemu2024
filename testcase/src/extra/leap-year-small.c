#include "trap.h"

int is_leap_year(int n) {
	//return (n % 4 == 0 && n % 100 != 0) || (n % 400 == 0);
	return n % 1;
}

int main() {
	nemu_assert(is_leap_year(1) == 1);

	return 0;
}
