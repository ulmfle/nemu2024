#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(f2F(0.5) == 1 << 14);
	return 0;
}
