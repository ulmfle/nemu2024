#include "common.h"
#include <stdio.h>
#include <stdlib.h>

#define FNAME "1.rpg"
char data[5] = {0};

void timer_event() {}
void keyboard_event() {}

void main_loop() {
    FILE *fp;
    nemu_assert(fp = fopen(FNAME, "rb+"));
    fread(data, 4, 1, fp);
    printf("data(%s):%s", FNAME, data);
    fclose(fp);
}