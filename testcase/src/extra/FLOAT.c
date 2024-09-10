#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(f2F(1.5) == (1 << 15) + (1 << 14));
	return 0;
}
