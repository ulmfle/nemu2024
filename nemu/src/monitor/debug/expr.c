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

int check_parentheses(int st, int ed) {
	int tki,pair=0;
	for (tki = st; tki <= ed; ++tki) {
		if (tokens[tki].type == LPR) ++pair;
		if (tokens[tki].type == RPR) --pair;
		if (pair<0) return false;
	}
	if (pair > 0) return false; else return true;
}

int skip_parentheses(int st, int is_rev) {
	int pr_pair = 0;
	int step = is_rev?-1:1;
	int l = is_rev?RPR:LPR,r = is_rev?LPR:RPR;
	do {
		if (tokens[st].type == l) ++pr_pair;
		if (tokens[st].type == r) --pr_pair;
		st+=step;
	} while (pr_pair&&st > -1&&st < nr_token);
	return st-step;
}

int find_op(int type_st, int type_ed, int st, int ed, int is_rev) {
	int tki;

	if (!is_rev) {
		for (tki = st; tki <= ed; ++tki) {
			if (tokens[tki].type == LPR) tki = skip_parentheses(tki,is_rev);
			if (tokens[tki].type >= type_st && tokens[tki].type <= type_ed) return tki;
		}
	} else {
		for (tki = ed; tki >= st; --tki) {
			if (tokens[tki].type == RPR) tki = skip_parentheses(tki,is_rev);
			if (tokens[tki].type >= type_st && tokens[tki].type <= type_ed) return tki;
		}
	}
	return -1;
}

int dom_op(int st, int ed) {
	int ret_1 = find_op(OR, OR, st, ed, 1);
	if (ret_1 != -1) return ret_1;
	int ret_2 = find_op(AND, AND, st, ed, 1);
	if (ret_2 != -1) return ret_2;
	int ret_3 = find_op(EQ, NEQ, st, ed, 1);
	if (ret_3 != -1) return ret_3;
	int ret_4 = find_op(ADD, SUB, st, ed, 1);
	if (ret_4 != -1) return ret_4;
	int ret_5 = find_op(MUL, DIV, st, ed, 1);
	if (ret_5 != -1) return ret_5;
	int ret_6 = find_op(REV, DEREF, st, ed, 0);
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
	}

	if (tokens[st].type == LPR && tokens[ed].type == RPR && check_parentheses(st+1, ed-1) == true) {
		return eval(st+1, ed-1, bad);

	} else {
		int op = dom_op(st, ed);

		switch(tokens[op].type) {
			case ADD:case SUB:case MUL:case DIV:
			case OR:case AND:case EQ:case NEQ:
			lvalue = eval(st, op-1, &bad_state_l);
			if (bad_state_l == 1 && tokens[op].type >= ADD && tokens[op].type <= MUL)
				tokens[op].type+=POS-ADD;

			case REV:case POS:case NEG:case DEREF:
			rvalue = eval(op+1, ed, &bad_state_r);
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

	if (check_parentheses(0, nr_token-1) == false) {
		printf("Bad expression : parentheses\n");
		return -1;
	}

	uint8_t bad_state;
	uint32_t ret = eval(0, nr_token-1, &bad_state);
	if (bad_state == 1) printf("Bad expression\n");

	*success = true;
	return ret;
}