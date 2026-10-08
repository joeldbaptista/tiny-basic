/* util.c: scanning and error helpers. */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basic.h"

static int reserved(const char *p, int n);

static const char *keywords[] = {
	"goto",
	"if",
	"print",
	"stop",
	"then",
	NULL
};

void
skipspace(void)
{
	while (*bufptr == ' ' || *bufptr == '\t')
		bufptr++;
}

/* reserved: return 1 if the n letters at p are a keyword, else return 0. */
static int
reserved(const char *p, int n)
{
	const char **k;

	for (k = keywords; *k; k++) {
		if ((int)strlen(*k) == n && strncmp(*k, p, n) == 0)
			return 1;
	}

	return 0;
}

void
die(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "bcom: line %d: ", lineno);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
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
		die("expected '%s'", s);
}

/* keyword: consume s and return 1 if the next word is s, else return 0. */
int
keyword(const char *s)
{
	size_t n;

	skipspace();
	n = strlen(s);
	if (strncmp(bufptr, s, n) != 0 || islower((unsigned char)bufptr[n]))
		return 0;
	bufptr += n;
	skipspace();

	return 1;
}

void
expectkw(const char *s)
{
	if (!keyword(s))
		die("expected '%s'", s);
}

/* name: consume a variable or label name; set p to it, return its length. */
int
name(char **p)
{
	int n;

	skipspace();
	*p = bufptr;
	while (islower((unsigned char)*bufptr))
		bufptr++;
	n = (int)(bufptr - *p);
	if (n == 0)
		die("expected a name");
	if (reserved(*p, n))
		die("'%.*s' is a reserved word", n, *p);
	skipspace();

	return n;
}

/* emitnum: print op followed by the number at the input. */
void
emitnum(const char *op)
{
	if (!isdigit((unsigned char)*bufptr))
		die("expected a number");
	printf("%s ", op);
	while (isdigit((unsigned char)*bufptr))
		putchar(*bufptr++);
	putchar('\n');
	skipspace();
}
