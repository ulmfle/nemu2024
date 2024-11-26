#include "device/video.h"

volatile int tick = 0;

void
timer_event(void) {
	tick ++;
}

void
keyboard_event(void){
}

void main_loop() {
    while (true) {
        int idx,jdx;
        // draw_string("Success!", 0, 0, 0x33);
        for (idx = 0; idx < SCR_HEIGHT/2; ++idx) {
            for (jdx = 0; jdx < SCR_WIDTH/2; ++jdx) {
                draw_pixel(idx, jdx, 0x33);
            }
        }
        display_buffer();
    }
}