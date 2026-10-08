/* stmt.c: code generation for Tiny Basic statements. */

#include <stdio.h>

#include "basic.h"

static void emitlabel(const char *op);

/* emitlabel: print op followed by the label name at the input. */
static void
emitlabel(const char *op)
{
	char *p;
	int n;

	n = name(&p);
	printf("%s %.*s\n", op, n, p);
}

/* dolabel: labelphrase = label ":" */
void
dolabel(const char *p, int n)
{
	printf("label %.*s\n", n, p);
}

/* dolet: letphrase = variable "=" expr */
void
dolet(const char *p, int n)
{
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

/* doif: ifphrase = "if" expr "then" gotophrase */
void
doif(void)
{
	expr();
	expectkw("then");
	expectkw("goto");
	emitlabel("jump");
}

void
dostop(void)
{
	printf("stop\n");
}

/* dogoto: gotophrase = "goto" label */
void
dogoto(void)
{
	emitlabel("goto");
}
