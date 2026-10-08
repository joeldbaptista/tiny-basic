/* util.c: scanning and error helpers. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic.h"

static void skipspace(void);

static void
skipspace(void)
{
	while (*bufptr == ' ')
		bufptr++;
}

void
die(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/* match: consume s and return 1 if the input starts with s, else return 0. */
int
match(const char *s)
{
	char *p;

	skipspace();
	for (p = bufptr; *s != '\0' && *s == *p; s++, p++)
		;
	if (*s != '\0')
		return 0;
	bufptr = p;
	skipspace();

	return 1;
}

void
expect(const char *s)
{
	if (!match(s))
		die("syntax error");
}

/* emitnum: print op followed by the number at the input. */
void
emitnum(const char *op)
{
	if (!isdigit((unsigned char)*bufptr))
		die("syntax error");
	printf("%s ", op);
	while (isdigit((unsigned char)*bufptr))
		putchar(*bufptr++);
	putchar('\n');
}
