#include "device/video.h"

extern char font8x8_basic[128][8];
volatile int tick = 0;

void
timer_event(void) {
	tick ++;
}

void
keyboard_event(void){
}

void draw_character_fixed(int ch, int x, int y, int color) {
    set_bp();
    int i,j;
    for (i = 0; i < 8; ++i) {
        printf("%08x:", font8x8_basic[ch][i]);
        for (j = 0; j < 8; ++j) {
            if ((font8x8_basic[ch][i] >> j)&1) {
                printf("00");
                draw_pixel(x+i, y+j, color);
            } else {
                printf("**");
            }
        }
        putchar('\n');
    }
    putchar('\n');
}

void main_loop() {
    while (true) {
        draw_character_fixed('S',0,0,0x33);
        draw_character_fixed('U',0,8,0x33);
        draw_character_fixed('C',0,16,0x33);
        draw_character_fixed('C',0,24,0x33);
        draw_character_fixed('E',0,30,0x33);
        draw_character_fixed('S',0,36,0x33);
        draw_character_fixed('S',0,40,0x33);
        display_buffer();
    }
}