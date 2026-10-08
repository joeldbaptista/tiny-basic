/* vm.h: bytecode format shared by the assembler and the virtual machine. */

/*
 * A bytecode file has a header and a code section.
 *
 * Header (12 bytes):
 *	4 bytes	magic "TBVM"
 *	4 bytes	number of variable slots, unsigned
 *	4 bytes	number of instructions, unsigned
 *
 * Code (5 bytes per instruction):
 *	1 byte	opcode
 *	4 bytes	operand, signed two's complement (0 when unused)
 *
 * All multi-byte integers are little-endian. The operand of fetch and
 * store is a variable slot. The operand of jump and goto is an
 * instruction index; the index equal to the number of instructions
 * halts the program.
 */

#define MAGIC   "TBVM"
#define MAGICSZ (sizeof(MAGIC) - 1)
#define MAXCODE 4096
#define MAXVARS 256

/* encoding of a 32-bit word */
#define WORDSZ   4               /* bytes in a word */
#define BYTEBITS 8               /* bits in a byte */
#define BYTEMASK 0xffu           /* low byte of a word */
#define SIGNBIT  0x80000000u     /* sign bit of a word */
#define WORDMOD  0x100000000LL   /* 2^32, number of word values */

enum {
	OP_GETNM,
	OP_FETCH,
	OP_STORE,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_NEG,
	OP_TSTEQ,
	OP_TSTNE,
	OP_TSTGT,
	OP_TSTLT,
	OP_TSTGE,
	OP_TSTLE,
	OP_JUMP,
	OP_GOTO,
	OP_PRINT,
	OP_STOP,
	NOPS
};
