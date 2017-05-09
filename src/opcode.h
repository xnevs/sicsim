#ifndef OPCODE_H
#define OPCODE_H

#include <stdlib.h>

enum opcode {
	/* load */
	OPCODE_LDA = 0x00,
	OPCODE_LDCH = 0x50,
	OPCODE_LDX = 0x04,
	OPCODE_LDL = 0x08,
	OPCODE_LDB = 0x68,
	OPCODE_LDS = 0x6C,
	OPCODE_LDF = 0x70,
	OPCODE_LDT = 0x74,

	OPCODE_LPS = 0xD0,

	/* store */
	OPCODE_STA = 0x0C,
	OPCODE_STCH = 0x54,
	OPCODE_STX = 0x10,
	OPCODE_STL = 0x14,
	OPCODE_STB = 0x78,
	OPCODE_STS = 0x7C,
	OPCODE_STF = 0x80,
	OPCODE_STT = 0x84,

	OPCODE_STI = 0xD4,
	OPCODE_STSW = 0xE8,

	/* registers */
	OPCODE_CLEAR = 0xB4,
	OPCODE_RMO = 0xAC,

	/* bit */
	OPCODE_AND = 0x40,
	OPCODE_OR = 0x44,
	OPCODE_SHIFTL = 0xA4,
	OPCODE_SHIFTR = 0xA8,

	/* compare */
	OPCODE_COMP = 0x28,
	OPCODE_COMPR = 0xA0,
	OPCODE_COMPF = 0x88,

	OPCODE_TIX = 0x2C,
	OPCODE_TIXR = 0xB8,

	/* jump */
	OPCODE_JEQ = 0x30,
	OPCODE_JGT = 0x34,
	OPCODE_JLT = 0x38,
	OPCODE_J = 0x3C,

	OPCODE_JSUB = 0x48,
	OPCODE_RSUB = 0x4C,

	/* artihmetic */
	OPCODE_ADD = 0x18,
	OPCODE_SUB = 0x1C,
	OPCODE_MUL = 0x20,
	OPCODE_DIV = 0x24,

	OPCODE_ADDR = 0x90,
	OPCODE_SUBR = 0x94,
	OPCODE_MULR = 0x98,
	OPCODE_DIVR = 0x9C,

	/* float */
	OPCODE_ADDF = 0x58,
	OPCODE_SUBF = 0x5C,
	OPCODE_MULF = 0x60,
	OPCODE_DIVF = 0x64,
	OPCODE_FLOAT = 0xC0,
	OPCODE_FIX = 0xC4,
	OPCODE_NORM = 0xC8,

	/* devices */
	OPCODE_RD = 0xD8,
	OPCODE_WD = 0xDC,
	OPCODE_TD = 0xE0,

	/* system */
	OPCODE_SSK = 0xEC,

	/* IO */
	OPCODE_SIO = 0xF0,
	OPCODE_HIO = 0XF4,
	OPCODE_TIO = 0xF8,

	/* other */
	OPCODE_SVC = 0xB0,

	/* directives */
	DIRECTIVE_START = 0x100,
	DIRECTIVE_END = 0x101,
	DIRECTIVE_ORG = 0x102,
	DIRECTIVE_EQU = 0x103,
	DIRECTIVE_BASE = 0x104,
	DIRECTIVE_NOBASE = 0x105,
	DIRECTIVE_LTORG = 0x106,

	/* storage */
	STORAGE_BYTE = 0x111,
	STORAGE_WORD = 0x112,
	STORAGE_RESB = 0x113,
	STORAGE_RESW = 0x114,
	STORAGE_FLOAT = 0x115,

	OPCODE_INVALID = 0x120,

	OPCODE_EXTENDED = 0x130
};

int opcode_to_str(enum opcode opcode, char str[], int n);

extern char *opcode_str[];

enum opcode opcode_parseMnemonic(char *mnemonic);
int opcode_getFormat(enum opcode opcode);
	
#endif
