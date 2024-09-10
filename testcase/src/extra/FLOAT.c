#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(F_div_F(f2F(8), f2F(4)) == f2F(0));
	return 0;
}
