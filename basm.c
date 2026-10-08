/* basm.c: assembles Tiny Basic stack machine assembly into bytecode. */

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

#define MAXLINE   256
#define MAXLABELS 1024
#define MAXNAME   32

enum {
	ARG_NONE,
	ARG_NUM,
	ARG_VAR,
	ARG_LABEL
};

typedef struct insn_def InsnDef;
struct insn_def {
	const char *name;
	int op;
	int arg;
};

typedef struct insn Insn;
struct insn {
	int op;
	long arg;
	int line;
};

typedef struct label Label;
struct label {
	char name[MAXNAME];
	int addr;   /* -1 until the label is defined */
};

static void die(const char *fmt, ...);
static const InsnDef *lookup(const char *name);
static long parsenum(const char *s);
static int varslot(const char *name);
static int labelslot(const char *name);
static void addlabel(const char *arg);
static void addinsn(const InsnDef *d, const char *arg);
static void assemble(void);
static void resolve(void);
static void put32(uint32_t v);
static void emit(void);

static const InsnDef defs[] = {
	{ "getnm", OP_GETNM, ARG_NUM },
	{ "fetch", OP_FETCH, ARG_VAR },
	{ "store", OP_STORE, ARG_VAR },
	{ "add",   OP_ADD,   ARG_NONE },
	{ "sub",   OP_SUB,   ARG_NONE },
	{ "mul",   OP_MUL,   ARG_NONE },
	{ "div",   OP_DIV,   ARG_NONE },
	{ "neg",   OP_NEG,   ARG_NONE },
	{ "tsteq", OP_TSTEQ, ARG_NONE },
	{ "tstne", OP_TSTNE, ARG_NONE },
	{ "tstgt", OP_TSTGT, ARG_NONE },
	{ "tstlt", OP_TSTLT, ARG_NONE },
	{ "tstge", OP_TSTGE, ARG_NONE },
	{ "tstle", OP_TSTLE, ARG_NONE },
	{ "jump",  OP_JUMP,  ARG_LABEL },
	{ "goto",  OP_GOTO,  ARG_LABEL },
	{ "print", OP_PRINT, ARG_NONE },
	{ "stop",  OP_STOP,  ARG_NONE },
};

static Insn code[MAXCODE];
static int ncode;
static char vars[MAXVARS][MAXNAME];
static int nvars;
static Label labels[MAXLABELS];
static int nlabels;
static int lineno;

static void
die(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "basm: ");
	if (lineno > 0)
		fprintf(stderr, "line %d: ", lineno);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static const InsnDef *
lookup(const char *name)
{
	size_t i;

	for (i = 0; i < sizeof(defs) / sizeof(defs[0]); i++) {
		if (strcmp(defs[i].name, name) == 0)
			return &defs[i];
	}

	return NULL;
}

static long
parsenum(const char *s)
{
	char *end;
	long v;

	errno = 0;
	v = strtol(s, &end, 10);
	if (*end != '\0')
		die("bad number '%s'", s);
	if (errno == ERANGE || v < INT32_MIN || v > INT32_MAX)
		die("number out of range '%s'", s);

	return v;
}

/* varslot: return the slot of variable name, allocating it if new. */
static int
varslot(const char *name)
{
	int i;

	for (i = 0; i < nvars; i++) {
		if (strcmp(vars[i], name) == 0)
			return i;
	}
	if (nvars == MAXVARS)
		die("too many variables");
	if (strlen(name) >= MAXNAME)
		die("variable name too long '%s'", name);
	strcpy(vars[nvars], name);

	return nvars++;
}

/* labelslot: return the slot of label name, allocating it if new. */
static int
labelslot(const char *name)
{
	int i;

	for (i = 0; i < nlabels; i++) {
		if (strcmp(labels[i].name, name) == 0)
			return i;
	}
	if (nlabels == MAXLABELS)
		die("too many labels");
	if (strlen(name) >= MAXNAME)
		die("label name too long '%s'", name);
	strcpy(labels[nlabels].name, name);
	labels[nlabels].addr = -1;

	return nlabels++;
}

static void
addlabel(const char *arg)
{
	int i;

	if (!arg)
		die("label: missing operand");
	i = labelslot(arg);
	if (labels[i].addr >= 0)
		die("duplicate label '%s'", arg);
	labels[i].addr = ncode;
}

static void
addinsn(const InsnDef *d, const char *arg)
{
	Insn *in;

	if (d->arg == ARG_NONE && arg)
		die("%s: unexpected operand", d->name);
	if (d->arg != ARG_NONE && !arg)
		die("%s: missing operand", d->name);
	if (ncode == MAXCODE)
		die("too many instructions");

	in = &code[ncode++];
	in->op = d->op;
	in->line = lineno;
	switch (d->arg) {
	case ARG_NONE:
		in->arg = 0;
		break;
	case ARG_VAR:
		in->arg = varslot(arg);
		break;
	case ARG_NUM:
		in->arg = parsenum(arg);
		break;
	case ARG_LABEL:
		in->arg = labelslot(arg);
		break;
	}
}

static void
assemble(void)
{
	char buf[MAXLINE];
	char *op, *arg;
	const InsnDef *d;

	while (fgets(buf, sizeof(buf), stdin)) {
		lineno++;
		if (!strchr(buf, '\n') && !feof(stdin))
			die("line too long");
		if (!(op = strtok(buf, " \t\n")))
			continue;
		arg = strtok(NULL, " \t\n");
		if (strtok(NULL, " \t\n"))
			die("%s: too many operands", op);
		if (strcmp(op, "label") == 0) {
			addlabel(arg);
			continue;
		}
		if (!(d = lookup(op)))
			die("unknown instruction '%s'", op);
		addinsn(d, arg);
	}
	if (ferror(stdin))
		die("read error");
}

/* resolve: replace label slots with instruction addresses. */
static void
resolve(void)
{
	Label *l;
	int i;

	for (i = 0; i < ncode; i++) {
		if (code[i].op != OP_JUMP && code[i].op != OP_GOTO)
			continue;
		lineno = code[i].line;
		l = &labels[code[i].arg];
		if (l->addr < 0)
			die("undefined label '%s'", l->name);
		code[i].arg = l->addr;
	}
	lineno = 0;
}

static void
put32(uint32_t v)
{
	int i;

	for (i = 0; i < WORDSZ; i++)
		putchar((v >> (BYTEBITS * i)) & BYTEMASK);
}

static void
emit(void)
{
	int i;

	fwrite(MAGIC, 1, MAGICSZ, stdout);
	put32(nvars);
	put32(ncode);
	for (i = 0; i < ncode; i++) {
		putchar(code[i].op);
		put32((uint32_t)code[i].arg);
	}
	if (fflush(stdout) == EOF || ferror(stdout))
		die("write error");
}

int
main(int argc, char *argv[])
{
	(void)argv;
	if (argc != 1) {
		fprintf(stderr, "usage: basm < file.asm > file.bc\n");
		return 1;
	}
	assemble();
	resolve();
	emit();

	return 0;
}
