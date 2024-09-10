#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long r = (long long)Fabs(a)*(long long)Fabs(b);
	r >>= 16;
	return (FLOAT)r + (a & (1 << 31)) ^ (b & (1 << 31));
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* Dividing two 64-bit integers needs the support of another library
	 * `libgcc', other than newlib. It is a dirty work to port `libgcc'
	 * to NEMU. In fact, it is unnecessary to perform a "64/64" division
	 * here. A "64/32" division is enough.
	 *
	 * To perform a "64/32" division, you can use the x86 instruction
	 * `div' or `idiv' by inline assembly. We provide a template for you
	 * to prevent you from uncessary details.
	 *
	 *     asm volatile ("??? %2" : "=a"(???), "=d"(???) : "r"(???), "a"(???), "d"(???));
	 *
	 * If you want to use the template above, you should fill the "???"
	 * correctly. For more information, please read the i386 manual for
	 * division instructions, and search the Internet about "inline assembly".
	 * It is OK not to use the template above, but you should figure
	 * out another way to perform the division.
	 */
	long long a_un = (long long)Fabs(a) << 16;
	FLOAT b_un = Fabs(b);
	FLOAT r;

	asm volatile ("div %2" : "=a"(r) : "a"((int)(a_un >> 32)), "r"(b_un) , "d"((int)(a_un)));

	return r + (a & (1 << 31)) ^ (b & (1 << 31));
}

FLOAT f2F(float a) {
	/* You should figure out how to convert `a' into FLOAT without
	 * introducing x87 floating point instructions. Else you can
	 * not run this code in NEMU before implementing x87 floating
	 * point instructions, which is contrary to our expectation.
	 *
	 * Hint: The bit representation of `a' is already on the
	 * stack. How do you retrieve it to another variable without
	 * performing arithmetic operations on it directly?
	 */

	int _a;
	asm volatile ("mov %1, %0" : "=a"(_a) : "r"(a));

	FLOAT of = (~0u >> 1) + (_a & (1 << 31));
	char E_pre = (_a & (0xff << 23)) >> 23;

	if (E_pre == 0xff) return of;

	char E = E_pre ? (E_pre - 127) : 1 - 127;
	int M = _a & 0x7fffff + (E_pre > 0 ? (1 << 23) : 0);
	if (!M) return 0;

	FLOAT R = E >= 8 ? M << (E-8) : M >> (8-E);
	return R + (_a & (1 << 31));
}

FLOAT Fabs(FLOAT a) {
	return a & (~0u >> 1);
}

/* Functions below are already implemented */

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

