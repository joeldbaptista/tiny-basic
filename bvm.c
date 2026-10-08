/* bvm.c: runs Tiny Basic bytecode on a stack machine. */

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

#define MAXSTACK 256

typedef struct insn Insn;
struct insn {
	int op;
	long long arg;
};

static void die(const char *fmt, ...);
static uint32_t get32(FILE *fp);
static void load(FILE *fp);
static void push(long long v);
static long long pop(void);
static void run(void);

static Insn code[MAXCODE];
static uint32_t ncode;
static long long vars[MAXVARS];
static uint32_t nvars;
static long long stack[MAXSTACK];
static int sp;

static void
die(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "bvm: ");
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static uint32_t
get32(FILE *fp)
{
	uint32_t v;
	int i, c;

	v = 0;
	for (i = 0; i < WORDSZ; i++) {
		if ((c = getc(fp)) == EOF)
			die("truncated file");
		v |= (uint32_t)c << (BYTEBITS * i);
	}

	return v;
}

/* load: read and validate a bytecode file. */
static void
load(FILE *fp)
{
	char magic[MAGICSZ];
	uint32_t i, u;
	int op;

	if (fread(magic, 1, MAGICSZ, fp) != MAGICSZ
	    || memcmp(magic, MAGIC, MAGICSZ) != 0)
		die("not a bytecode file");
	if ((nvars = get32(fp)) > MAXVARS)
		die("too many variables: %lu", (unsigned long)nvars);
	if ((ncode = get32(fp)) > MAXCODE)
		die("too many instructions: %lu", (unsigned long)ncode);

	for (i = 0; i < ncode; i++) {
		if ((op = getc(fp)) == EOF)
			die("truncated file");
		if (op >= NOPS)
			die("%lu: bad opcode %d", (unsigned long)i, op);
		u = get32(fp);
		code[i].op = op;
		code[i].arg = u;
		if (u & SIGNBIT)
			code[i].arg -= WORDMOD;

		switch (op) {
		case OP_FETCH: /* FALLTHROUGH */
		case OP_STORE:
			if (code[i].arg < 0 || code[i].arg >= nvars)
				die("%lu: bad variable slot", (unsigned long)i);
			break;
		case OP_JUMP: /* FALLTHROUGH */
		case OP_GOTO:
			if (code[i].arg < 0 || code[i].arg > ncode)
				die("%lu: bad jump target", (unsigned long)i);
			break;
		}
	}
	if (getc(fp) != EOF)
		die("trailing data");
	if (ferror(fp))
		die("read error");
}

static void
push(long long v)
{
	if (sp == MAXSTACK)
		die("stack overflow");
	stack[sp++] = v;
}

static long long
pop(void)
{
	if (sp == 0)
		die("stack underflow");

	return stack[--sp];
}

static void
run(void)
{
	uint32_t pc;
	long long a, b;
	Insn *in;

	pc = 0;
	while (pc < ncode) {
		in = &code[pc++];
		switch (in->op) {
		case OP_GETNM:
			push(in->arg);
			break;
		case OP_FETCH:
			push(vars[in->arg]);
			break;
		case OP_STORE:
			vars[in->arg] = pop();
			break;
		case OP_NEG:
			push(-pop());
			break;
		case OP_JUMP:
			if (pop())
				pc = in->arg;
			break;
		case OP_GOTO:
			pc = in->arg;
			break;
		case OP_PRINT:
			printf("%lld\n", pop());
			break;
		case OP_STOP:
			return;
		default:
			/* binary operators: b is the 1st pop, a is the 2nd */
			b = pop();
			a = pop();
			switch (in->op) {
			case OP_ADD:
				push(a + b);
				break;
			case OP_SUB:
				push(a - b);
				break;
			case OP_MUL:
				push(a * b);
				break;
			case OP_DIV:
				if (b == 0)
					die("%lu: division by zero",
					    (unsigned long)(pc - 1));
				if (a == LLONG_MIN && b == -1)
					die("%lu: overflow",
					    (unsigned long)(pc - 1));
				push(a / b);
				break;
			case OP_TSTEQ:
				push(a == b);
				break;
			case OP_TSTNE:
				push(a != b);
				break;
			case OP_TSTGT:
				push(a > b);
				break;
			case OP_TSTLT:
				push(a < b);
				break;
			case OP_TSTGE:
				push(a >= b);
				break;
			case OP_TSTLE:
				push(a <= b);
				break;
			default:
				abort(); /* NOTREACHED: load() checks opcodes */
			}
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc > 2) {
		fprintf(stderr, "usage: bvm [file.bc]\n");
		return 1;
	}
	if (argc == 1) {
		fp = stdin;
	} else if (!(fp = fopen(argv[1], "rb"))) {
		die("cannot open %s", argv[1]);
	}
	load(fp);
	if (fp != stdin)
		fclose(fp);
	run();

	return 0;
}
