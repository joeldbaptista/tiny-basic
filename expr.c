/* expr.c: code generation for Tiny Basic expressions. */

#include <ctype.h>
#include <stdio.h>

#include "basic.h"

static void expr1(void);
static void expr2(void);
static void expr3(void);
static void getvar(void);

/* expr: exp1 (relop exp1)? */
void
expr(void)
{
	const char *op;

	expr1();
	if (match("=="))
		op = "tsteq";
	else if (match("!="))
		op = "tstne";
	else if (match("<="))
		op = "tstle";
	else if (match(">="))
		op = "tstge";
	else if (match("<"))
		op = "tstlt";
	else if (match(">"))
		op = "tstgt";
	else
		return;
	expr1();
	printf("%s\n", op);
}

/* expr1: exp2 (addop exp2)* */
static void
expr1(void)
{
	expr2();
	for (;;) {
		if (match("+")) {
			expr2();
			printf("add\n");
		} else if (match("-")) {
			expr2();
			printf("sub\n");
		} else {
			return;
		}
	}
}

/* expr2: exp3 (mulop exp3)* */
static void
expr2(void)
{
	expr3();
	for (;;) {
		if (match("*")) {
			expr3();
			printf("mul\n");
		} else if (match("/")) {
			expr3();
			printf("div\n");
		} else {
			return;
		}
	}
}

/* expr3: "-"? ("(" expr ")" | number | variable) */
static void
expr3(void)
{
	int neg;

	neg = match("-");
	if (match("(")) {
		expr();
		expect(")");
	} else if (isdigit((unsigned char)*bufptr)) {
		emitnum("getnm");
	} else {
		getvar();
	}
	if (neg)
		printf("neg\n");
}

static void
getvar(void)
{
	if (!islower((unsigned char)*bufptr))
		die("syntax error");
	printf("fetch ");
	while (islower((unsigned char)*bufptr))
		putchar(*bufptr++);
	putchar('\n');
}
