#include "trap.h"
#include <string.h>

int main() {
	nemu_assert(strcmp("aaa", "aaa") == 1);
	return 0;
}
