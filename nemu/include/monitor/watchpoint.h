#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	uint32_t v_prev;
	char *expr;
	struct watchpoint *next;
	/* TODO: Add more members if necessary */
} WP;

WP *make_wp(char *expr);
WP *get_wp_head();
int del_wp(int);

#endif