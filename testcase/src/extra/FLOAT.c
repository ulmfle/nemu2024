#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(F_div_F(f2F(3), f2F(1)) == f2F(6));
	return 0;
}
