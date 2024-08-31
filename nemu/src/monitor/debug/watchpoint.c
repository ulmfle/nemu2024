#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 3

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

WP *new_wp() {
	Assert(free_ != NULL, "No empty watchpoints!");
	WP *prev = free_;
	free_ = free_->next;
	prev->next = NULL;
	if (head == NULL) {
		head = prev;
		head->NO = 0;
	} else {
		WP *wpidx = head;
		while (wpidx->next != NULL) wpidx = wpidx->next;
		wpidx->next = prev;
		prev->NO = wpidx->NO + 1;
	}
	return prev;
}

void free_wp(WP* wp) {
	WP *wpidx = head;
	while (wpidx->next != NULL && wpidx->next != wp) wpidx = wpidx->next;
	wpidx->next = wp->next;

	if (wp == head) {
		head = wpidx = head->next;
		while (wpidx != NULL) {
			--wpidx->NO;
			wpidx = wpidx->next;
		}
	} else {
		while (wpidx->next != NULL) {
			wpidx->next->NO = wpidx->NO+1;
			wpidx = wpidx->next;
		}
	}

	wp->next = NULL;
	strcpy(wp->expr, "");

	WP *fwpi = free_;
	while (fwpi->next != NULL) fwpi = fwpi->next;
	fwpi->next = wp;
}

int del_wp(uint32_t n) {
	WP *wpidx = head;
	while (wpidx != NULL && wpidx->NO != n) wpidx = wpidx->next;
	if (wpidx == NULL) return -1;
	free_wp(wpidx);
	return 0;
}

WP *get_wp_head() {
	return head;
}

WP *make_wp(char *s_expr) {
	bool expr_succ;
	uint32_t pr_v = expr(s_expr, &expr_succ);
	if (expr_succ == false) {
		return NULL;
	}
	WP *nwp = new_wp();
	strcpy(nwp->expr, s_expr);
	nwp->v_prev = pr_v;
	return nwp;
}