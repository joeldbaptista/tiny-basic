/* main.c: reads Tiny Basic lines and dispatches each statement. */

#include <stdio.h>

#include "basic.h"

static int readline(void);
static void doline(void);

char *bufptr;
int lineno;
static char inbuf[MAXLINE];

/* readline: read the next line into inbuf; return 0 at end of input. */
static int
readline(void)
{
	int c, i;

	bufptr = inbuf;
	i = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (i == MAXLINE - 1)
			die("line too long");
		inbuf[i++] = c;
	}
	inbuf[i] = '\0';
	lineno++;

	return c != EOF || i > 0;
}

/* doline: line = (statement | labelphrase | comment)? */
static void
doline(void)
{
	char *p;
	int n;

	skipspace();
	if (*bufptr == '\0' || *bufptr == '#')
		return;
	if (keyword("print")) {
		doprint();
	} else if (keyword("if")) {
		doif();
	} else if (keyword("stop")) {
		dostop();
	} else if (keyword("goto")) {
		dogoto();
	} else {
		n = name(&p);
		if (match(":"))
			dolabel(p, n);
		else
			dolet(p, n);
	}
	if (*bufptr != '\0')
		die("unexpected '%s'", bufptr);
}

int
main(void)
{
	while (readline())
		doline();

	return 0;
}
