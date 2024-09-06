#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_p(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_debug(char *args);

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Allow the program to execute N instructions step by step before pausing execution. If N is not provided, the default is 1.", cmd_si},
    { "info", "Enter \'r\' to print the register status, and enter \'w\' to print the watchpoint information", cmd_info},
    { "x", "Evaluate the expression EXPR, use the result as the starting memory address, and output the next N 4-byte values in hexadecimal format.", cmd_x},
	{ "p", "Evaluate the expression", cmd_p},
	{ "w", "Create watchpoints", cmd_w},
	{ "d", "Remove watchpoints", cmd_d},
	{ "debug", "debug", cmd_debug}
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_p(char *args) {
	if (args == NULL) {
		printf("Empty expression.\n");
		return 0;
	}
	bool succ = false;
	int ret;
	ret = expr(args, &succ);
	if (succ == false) {
		return 0;
	}
	printf("0x%08x(%ld)\n", ret, (long)ret);
	return 0;
}

static int cmd_si(char *args) {
	if (args == NULL) {
		cpu_exec(1);
		return 0;
	}

	uint32_t n = 0;

	sscanf(args,"%d",&n);

	cpu_exec(n);

	return 0;
}

static int cmd_info(char *args) {
    char subcmd;
    sscanf(args,"%c",&subcmd);

    switch (subcmd) {
        case 'r': {
            int reglen = sizeof(cpu.gpr) / sizeof(cpu.gpr[0]);
            int idx;
            for (idx = 0; idx < reglen; ++idx)
                printf("%s\t\t0x%08x\t\t%d\n", regsl[idx], reg_l(idx), reg_l(idx));
            printf("%s\t\t0x%08x\t\t%d\n", "eip", cpu.eip, cpu.eip);
			printf("%s\t\t0x%08x\t\t%d\n", "eflags", cpu.eflags.val, cpu.eflags.val);
            break;
        }

        case 'w': {
			WP *wph = get_wp_head(), *wpidx;
			if (wph != NULL) printf("%8s\t%s\t\t%s\n", "NO", "address", "enable"); else return 0;
			for (wpidx = wph; wpidx != NULL; wpidx = wpidx->next) {
				printf("%8u\t%s\t0x%08x\n",wpidx->NO, wpidx->expr, expr(wpidx->expr, NULL));
			}
		}

        break;
    }

    return 0;
}

static int cmd_x(char *args) {
    int n,len = 4;
    char *s_expr = NULL;
    swaddr_t _expr = 0;
	bool expr_succ;

    sscanf(args, "%d", &n);

    args = strtok(args, " ");
    s_expr = args + strlen(args) + 1;
	_expr = expr(s_expr, &expr_succ);
	if (expr_succ == false) {
		return 0;
	}

    while (n>0) {
        printf("0x%08x: ",_expr);
        int t;
        for (t = 0; t < 4 && n > 0; ++t, --n, _expr+=len) {
            printf("0x%08x ", swaddr_read(_expr, len));
        }
        putchar('\n');
    }

    return 0;

}

static int cmd_w(char *args) {
	WP *wp = make_wp(args);
	if (wp != NULL) printf("Set watchpoint #%u\n", wp->NO);
	return 0;
}

static int cmd_d(char *args) {
	uint32_t n;
	sscanf(args, "%u", &n);
	int ret = del_wp(n);
	if (ret == -1) printf("Watchpoint #%u does not exist\n", n);
	return 0;
}

static int cmd_debug(char *args) {
	while (1) {
		cmd_info("r");
		cpu_exec(1);
	}
	return 0;
}


void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if (cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
