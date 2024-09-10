#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(F_div_F(f2F(4), f2F(1)) == f2F(4));
	return 0;
}
