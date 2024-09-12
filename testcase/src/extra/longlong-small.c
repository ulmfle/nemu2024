#include "trap.h"

long long add(long long a, long long b) {
	long long c = a + b;
	return c;
}

long long res;
int i, j, ans_idx = 0;
int loop = 0;
long long test_data[] = {1LL,2LL,3LL};
long long ans[] = {2LL,3LL,4LL,3LL,4LL,5LL,4LL,5LL,6LL};
#define NR_DATA (sizeof(test_data) / sizeof(test_data[0]))

int main() {
	//set_bp();
	for(i = 0; i < NR_DATA; i ++) {
		for(j = 0; j < NR_DATA; j ++) {
			res = add(test_data[i], test_data[j]);
			//set_bp();
			nemu_assert(res == ans[ans_idx ++]);
			loop ++;
			//set_bp();
		}
	}

	nemu_assert(loop == NR_DATA * NR_DATA);

	return 0;
}