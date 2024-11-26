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
    draw_string("Success!", 0, 0, 0x33);
    display_buffer();
}