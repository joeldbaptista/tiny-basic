/* main.c: reads Tiny Basic lines and dispatches each statement. */

#include <stdio.h>

#include "basic.h"

static int readline(void);

char *bufptr;
static char inbuf[MAXLINE];

static int
readline(void)
{
	int c, i;

	bufptr = inbuf;
	for (i = 0; (c = getchar()) != '\n' && c != EOF; i++) {
		if (i == MAXLINE - 1)
			die("line too long");
		inbuf[i] = c;
	}
	inbuf[i] = '\0';

	return i > 0;
}

int
main(void)
{
	while (readline()) {
		emitnum("label");
		if (match("let"))
			dolet();
		else if (match("print"))
			doprint();
		else if (match("if"))
			doif();
		else if (match("stop"))
			dostop();
		else if (match("goto"))
			dogoto();
		else
			dolet();
		if (*bufptr != '\0')
			die("syntax error");
	}

	return 0;
}
