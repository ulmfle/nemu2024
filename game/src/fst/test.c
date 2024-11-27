#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#define str(x) #x
#define FNAME str(1.rpg)
char data[5] = {0};

void main_loop() {
    FILE *fp;
    nemu_assert(fp = fopen(FNAME, "rb+"));
    nemu_assert(fread(data, 4, 1, fp) == 4);
    printf("data(%s):%s", FNAME, data);
    fclose(fp);
}