#include "hal.h"

#define NR_KEYS 18

enum {KEY_STATE_EMPTY, KEY_STATE_WAIT_RELEASE, KEY_STATE_RELEASE, KEY_STATE_PRESS};

/* Only the following keys are used in NEMU-PAL. */
static const int keycode_array[] = {
	K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE,
	K_RETURN, K_SPACE, K_PAGEUP, K_PAGEDOWN, K_r,
	K_a, K_d, K_e, K_w, K_q,
	K_s, K_f, K_p
};

static int key_state[NR_KEYS];

static inline int
get_keycode(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return keycode_array[index];
}

static inline int
query_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	return key_state[index];
}

static inline void
release_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_WAIT_RELEASE;
}

static inline void
clear_key(int index) {
	assert(index >= 0 && index < NR_KEYS);
	key_state[index] = KEY_STATE_EMPTY;
}

void
keyboard_event(int scan_code) {
	/* TODO: Fetch the scancode and update the key states. */
	int idx, rel = scan_code & 0x80;

	for (idx = 0; idx < NR_KEYS && get_keycode(idx) != scan_code; ++idx);

	if (key_state[idx] == KEY_STATE_EMPTY)
		key_state[idx] = KEY_STATE_PRESS;
	else if (key_state[idx] == KEY_STATE_PRESS)
		key_state[idx] = KEY_STATE_WAIT_RELEASE;
	else if (key_state[idx] == KEY_STATE_WAIT_RELEASE && rel)
		key_state[idx] = KEY_STATE_RELEASE;
	//assert(0);
}

bool 
process_keys(void (*key_press_callback)(int), void (*key_release_callback)(int)) {
	cli();
	/* TODO: Traverse the key states. Find a key just pressed or released.
	 * If a pressed key is found, call `key_press_callback' with the keycode.
	 * If a released key is found, call `key_release_callback' with the keycode.
	 * If any such key is found, the function return true.
	 * If no such key is found, the function return false.
	 * Remember to enable interrupts before returning from the function.
	 */
	int idx;
	for (idx = 0; idx < NR_KEYS; ++idx) {
		if (query_key(idx) == KEY_STATE_PRESS)
			key_press_callback(get_keycode(idx));
		else if (query_key(idx) == KEY_STATE_RELEASE)
			key_release_callback(get_keycode(idx));
		else
			continue;
		clear_key(idx);
		sti();
		return 1;
	}
	//assert(0);
	sti();
	return 0;
}
