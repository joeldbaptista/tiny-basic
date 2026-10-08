/* stmt.c: code generation for Tiny Basic statements. */

#include <ctype.h>
#include <stdio.h>

#include "basic.h"

void
dolet(void)
{
	char *p;
	int n;

	p = bufptr;
	while (islower((unsigned char)*bufptr))
		bufptr++;
	n = (int)(bufptr - p);
	if (n == 0)
		die("syntax error");
	expect("=");
	expr();
	printf("store %.*s\n", n, p);
}

void
doprint(void)
{
	expr();
	printf("print\n");
}

void
doif(void)
{
	expr();
	expect("then");
	emitnum("jump");
}

void
dostop(void)
{
	printf("stop\n");
}

void
dogoto(void)
{
	emitnum("goto");
}
