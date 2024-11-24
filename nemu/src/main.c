//
#include "common.h"
//
void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

#ifdef DEBUG
#include <setjmp.h>
jmp_buf restart_buf;
#endif

int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the `CPU_state' structure. */
	reg_test();

#ifdef DEBUG
	setjmp(restart_buf);
#endif

	/* Initialize the virtual computer system. */
	restart();

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
