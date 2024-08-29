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
	EQ,  NEQ,
	ADD, SUB,
	MUL, DIV,
	REV, POS, NEG, DEREF,
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
	{"\\|\\|"			  , OR	  },					// or
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
					/*case BIN:case OCT:*/case HEX:
					/*if (rules[i].token_type == OCT) si = 1; else */si = 2;

					case DEC:
						for (; si < substr_len && si < 32; ++si) {
							tokens[nr_token].str[si] = substr_start[si];
						}

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

int find_op(int type_st, int type_ed, int st, int ed, int is_rev) {
	int tki,step;
	int pr_pair = 0;

	step = is_rev?-1:1;
	for (tki = st; tki >= ed; tki+=step) {
		if (tokens[tki].type == LPR) {
			do {
				if (tokens[tki].type == LPR+is_rev) ++pr_pair;
				else if (tokens[tki].type == RPR-is_rev) --pr_pair;
				tki+=step;
			} while (pr_pair);
		}
		if (tokens[tki].type >= type_st && tokens[tki].type <= type_ed) return tki;
	}

	return -1;
}

int dom_op(int st, int ed) {
	int ret_1 = find_op(OR, OR, ed, st, 1);
	Log("ret_1:%d",ret_1);
	if (ret_1 != -1) return ret_1;
	int ret_2 = find_op(AND, AND, ed, st, 1);
	Log("ret_2:%d",ret_2);
	if (ret_2 != -1) return ret_2;
	int ret_3 = find_op(EQ, NEQ, ed, st, 1);
	Log("ret_3:%d",ret_3);
	if (ret_3 != -1) return ret_3;
	int ret_4 = find_op(ADD, SUB, ed, st, 1);
	Log("ret_4:%d",ret_4);
	if (ret_4 != -1) return ret_4;
	int ret_5 = find_op(MUL, DIV, ed, st, 1);
	Log("ret_5:%d",ret_5);
	if (ret_5 != -1) return ret_5;
	int ret_6 = find_op(REV, DEREF, st, ed, 0);
	Log("ret_6:%d",ret_6);
	if (ret_6 != -1) return ret_6;
	panic("Cannot find dom_op!\n");
	return -1;
}

uint32_t eval(int st, int ed, uint8_t *bad) {
	if (st>ed) {
		*bad = 1;
		return 0;
	}

	uint32_t value = 0, lvalue = 0, rvalue = 0;
	uint8_t bad_state_l = 0,bad_state_r = 0;

	if (st==ed) {
		switch (tokens[st].type) {
			case DEC:sscanf(tokens[st].str, "%u", &value); break;
			case HEX:sscanf(tokens[st].str + 2, "%x", &value); break;
		}
		return value;

	} else if (tokens[st].type == '(' && tokens[ed].type == ')') {
		return eval(st+1, ed-1, bad);

	} else {
		int op = dom_op(st, ed);

		switch(tokens[op].type) {
			case ADD:case SUB:case MUL:case DIV:
			case OR:case AND:case EQ:case NEQ:
			lvalue = eval(st, op-1, &bad_state_l);
			Assert(bad_state_l == 0, "Lvalue evaluation failed!\n");
			case REV:case POS:case NEG:case DEREF:
			rvalue = eval(op+1, ed, &bad_state_r);
			Assert(bad_state_r == 0, "Rvalue evaluation failed!\n");
		}

		switch(tokens[op].type) {
			case ADD: value = lvalue + rvalue; break;
			case SUB: value = lvalue - rvalue; break;
			case MUL: value = lvalue * rvalue; break;
			case DIV: value = lvalue / rvalue; break;
			case OR : value = lvalue || rvalue; break;
			case AND: value = lvalue && rvalue; break;
			case EQ : value = lvalue == rvalue; break;
			case NEQ : value = lvalue != rvalue; break;
			case REV: value = !rvalue; break;
			case POS: value = 0+rvalue; break;
			case NEG: value = 0-rvalue; break;
			case DEREF: value = swaddr_read(rvalue, 4); break;
		}

	}
	return value;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	//preprocess : check POSitive, NEGative, DEREFerence
	int tki = 0;
	for (;tki < nr_token; ++tki) {
		if (tokens[tki].type >= POS && tokens[tki].type <= DEREF && (tki = 0|| (tokens[tki-1].type < BIN && tokens[tki+1].type >= BIN)))
		tokens[tki].type+=POS-ADD;
	}

	uint8_t bad_state;
	uint32_t ret = eval(0, nr_token-1, &bad_state);
	return ret;
}