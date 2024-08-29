#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,
	LPR, RPR,

	OR,
	AND,
	REV,
	EQ,  NEQ,
	ADD, SUB,
	MUL, DIV,
	DEREF,
	BIN, OCT, DEC, HEX
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +"         		  , NOTYPE},				    // spaces
	{"\\("				  , LPR   },					// left parenthesis
	{"\\)"				  , RPR   },					// right parenthesis
	//{"\\|\\|"			  , OR	  },					// or
	{"&&"				  , AND   },					// and
	{"!="				  , NEQ   },					// not equal
	{"=="       		  , EQ    },					// equal
	{"!" 				  , REV   },					// reverse
	{"\\+"        		  , ADD   },					// plus
	{"-"          		  , SUB   },					// minus
	{"\\*"         		  , MUL   },					// multiply
	{"/"          		  , DIV   },					// divided by
	{"0b[01]+"			  , BIN   },					// binary number
	{"0[0-7]+"			  , OCT	  },					// octal number
	{"0[xX][0-9a-fA-F]+"  , HEX   },					// hexademical number
	{"([1-9][0-9]+)|[0-9]", DEC   }						// decimal number
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				int si = 0;
				switch(rules[i].token_type) {
					case HEX:
						si = 2;
					case DEC:
						for (; si < substr_len && si < 32; ++si)
							tokens[nr_token].str[si] = substr_start[si];

					case LPR:case RPR:case OR :case AND:case ADD:case SUB:
					case MUL:case DIV:case EQ :case NEQ:case REV:
						tokens[nr_token++].type = rules[i].token_type;
					case NOTYPE:
						break;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	return 0;
}

uint32_t eval(int st, int ed, uint8_t *bad) {
	if (st>ed) {
		*bad = 1;
		return 0;
	}

	uint32_t value;
	//uint8_t bad_state_l = 0,bad_state_r = 0;

	if (st==ed) {
		sscanf(tokens[st].str, "%u", &value);
		return value;
	} else if (tokens[st].type == '(' && tokens[ed].type == ')') {
		eval(st+1, ed-1, bad);
	} else {
		
	}
	return 0;
}

int dom_op(int st, int ed) {
	return ed;
}