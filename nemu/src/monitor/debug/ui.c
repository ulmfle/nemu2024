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

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

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
    	{ "x", "Evaluate the expression EXPR, use the result as the starting memory address, and output the next N 4-byte values in hexadecimal format.", cmd_x}

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

static int cmd_si(char *args) {
	if (args == NULL) {
		cpu_exec(1);
		return 0;
	}

	uint32_t n = 0;

	sscanf(args,"%d",&n);
	
	while (n--)
	    cpu_exec(1);
	
	return 0;
} 

static int cmd_info(char *args) {
    char subcmd;
    sscanf(args,"%c",&subcmd);

    switch (subcmd) {
        case 'r': {
            int idx; 
            for (idx = 0; idx < 8; ++idx)  
                printf("%s\t\t0x%08x\t\t%d\n", regsl[idx], reg_l(idx), reg_l(idx));
            printf("%s\t\t0x%08x\t\t%d\n", "eip", cpu.eip, cpu.eip);
            break;
        }

        case 'w':

        break;
    }

    return 0;
}

static int cmd_x(char *args) {
    int n;
    char *s_expr = NULL;
    swaddr_t expr = 0;

    printf("str:%s\n",args);
    
    sscanf(args,"%d%s", &n, s_expr);
  
    printf("num:%d,str:%c", n, *s_expr);
    
    assert(0);
   /* 
    if (s_expr[0] != '0' && s_expr[1] != 'x') {
        printf("Expression must be hexdecimal format!\n");
        return 0;
    }
    
    sscanf(s_expr + 2,"%x", &expr);
   */ 
    printf("%#x",expr);
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
