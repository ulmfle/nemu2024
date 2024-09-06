#include "trap.h"

#define N 3

int d[N][N] = {{1, 2, 3},{4, 5, 6},{7, 8, 9}};
int k[N] = {0};
int ans[N] = {12, 15, 18};

int main() {
    int i,j;
    for (i = 0; i < N; ++i) {
        for (j = N-1; j >= 0; ++j) k[i] += d[j][i];
        nemu_assert(ans[i]==k[i]);
    }
    return 0;
}