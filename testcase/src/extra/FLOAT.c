#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(f2F(0.5) == 1 << 15);
	return 0;
}
