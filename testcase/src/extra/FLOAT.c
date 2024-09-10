#include "trap.h"
#include "FLOAT.h"

int main() {
    nemu_assert(F_div_F(f2F(1.5), f2F(0.5)) == f2F(3));
	return 0;
}
