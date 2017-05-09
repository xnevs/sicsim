#include "disasm.h"

#include "Machine.h"
#include "disasm.h"
#include "opcode.h"
#include "common.h"

#include <stdio.h>

//struct {
//	char *name;
//	int format;
//} opcode_name[0x3F] = {
//	{"LDA",3},
//	{"LDX",3},
//	{"LDL",3},
//	{"STA",3},
//	{"STX",3},
//	{"STL",3},
//	{"ADD",3},
//	{"SUB",3},
//	{"MUL",3},
//	{"DIV",3},
//	{"COMP",3},
//	{"TIX",3},
//	{"JEQ",3},
//	{"JGT",3},
//	{"JLT",3},
//	{"J",3},
//	{"AND",3},
//	{"OR",3},
//	{"JSUB",3},
//	{"RSUB",3},
//	{"LDCH",3},
//	{"STCH",3},
//	{"ADDF",3},
//	{"SUBF",3},
//	{"MULF",3},
//	{"DIVF",3},
//	{"LDB",3},
//	{"LDS",3},
//	{"LDF",3},
//	{"LDT",3},
//	{"STB",3},
//	{"STS",3},
//	{"STF",3},
//	{"STT",3},
//	{"COMPF",3},
//	{NULL,0},
//
//	{"ADDR",2},
//	{"SUBR",2},
//	{"MULR",2},
//	{"DIVR",2},
//	{"COMPR",2},
//	{"SHIFTL",2},
//	{"SHIFTR",2},
//	{"RMO",2},
//	{"SVC",2},
//	{"CLEAR",2},
//	{"TIXR",2},
//	{NULL,0},
//
//	{"FLOAT",1},
//	{"FIX",1},
//	{"NORM",1},
//	{NULL,0},
//
//	{"LPS",3},
//	{"STI",3},
//	{"RD",3},
//	{"WD",3},
//	{"TD",3},
//	{NULL,0},
//
//	{"STSW",3},
//	{"SSK",3},
//	{"SIO",1},
//	{"HIO",1},
//	{"TIO",1}
//};

char *regs[9] = {"A","X","L","B","S","T","F","SW","PC"};

int disasmF1(enum opcode opcode) {
	switch(opcode) {
		case OPCODE_FLOAT:
			printf("%6s", "FLOAT");
			break;
		case OPCODE_FIX:
			printf("%6s", "FIX");
			break;
		case OPCODE_NORM:
			printf("%6s", "NORM");
			break;
		case OPCODE_SIO:
			printf("%6s", "SIO");
			break;
		case OPCODE_HIO:
			printf("%6s", "HIO");
			break;
		case OPCODE_TIO:
			printf("%6s", "TIO");
			break;
		default:
			return 1;
	}
	return 0;
}
int disasmF2(enum opcode opcode, long operand) {
	int v1 = (operand & 0xF0) >> 4;
	int v2 = operand & 0x0F;
	switch(opcode) {
		case OPCODE_CLEAR: {
			printf("%6s   %s", "CLEAR", regs[v1]);
			break;
		}
		case OPCODE_RMO: {
			printf("%6s   %s %s", "RMO", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_SHIFTL: {
			printf("%6s   %s %d", "SHIFTL", regs[v1], v2);
			break;
		}
		case OPCODE_SHIFTR: {
			printf("%6s   %s %d", "SHIFTR", regs[v1], v2);
			break;
		}
		case OPCODE_COMPR: {
			printf("%6s   %s %s", "COMPR", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_TIXR: {
			printf("%6s   %s", "TIXR", regs[v1]);
			break;
		}
		case OPCODE_ADDR: {
			printf("%6s   %s %s", "ADDR", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_SUBR: {
			printf("%6s   %s %s", "SUBR", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_MULR: {
			printf("%6s   %s %s", "MULR", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_DIVR: {
			printf("%6s   %s %s", "DIVR", regs[v1], regs[v2]);
			break;
		}
		case OPCODE_SVC: {
			printf("%6s", "SVC");
			break;
		}
		default:
			return 1;
	}
	return 0;
}
int disasmSICF3F4(enum opcode opcode, int ni, long operand, int flags) {
	switch(opcode) {
		case OPCODE_LDA: {
			printf("%6s", "LDA");
			break;
		}
		case OPCODE_LDCH: {
			printf("%6s", "LDCH");
			break;
		}
		case OPCODE_LDX: {
			printf("%6s", "LDX");
			break;
		}
		case OPCODE_LDL: {
			printf("%6s", "LDL");
			break;
		}
		case OPCODE_LDB: {
			printf("%6s", "LDB");
			break;
		}
		case OPCODE_LDS: {
			printf("%6s", "LDS");
			break;
		}
		case OPCODE_LDF: {
			printf("%6s", "LDF");
			break;
		}
		case OPCODE_LDT: {
			printf("%6s", "LDT");
			break;
		}
		case OPCODE_LPS: {
			printf("%6s", "LPS");
			break;
		}
		case OPCODE_STA: {
			printf("%6s", "STA");
			break;
		}
		case OPCODE_STCH: {
			printf("%6s", "STCH");
			break;
		}
		case OPCODE_STX: {
			printf("%6s", "STX");
			break;
		}
		case OPCODE_STL: {
			printf("%6s", "STL");
			break;
		}
		case OPCODE_STB: {
			printf("%6s", "STB");
			break;
		}
		case OPCODE_STS: {
			printf("%6s", "STS");
			break;
		}
		case OPCODE_STF: {
			printf("%6s", "STF");
			break;
		}
		case OPCODE_STT: {
			printf("%6s", "STT");
			break;
		}
		case OPCODE_STI: {
			printf("%6s", "STI");
			break;
		}
		case OPCODE_STSW: {
			printf("%6s", "STSW");
			break;
		}
		case OPCODE_AND: {
			printf("%6s", "AND");
			break;
		}
		case OPCODE_OR: {
			printf("%6s", "OR");
			break;
		}
		case OPCODE_COMP: {
			printf("%6s", "COMP");
			break;
		}
		case OPCODE_COMPF: {
			printf("%6s", "COMPF");
			break;
		}
		case OPCODE_TIX: {
			printf("%6s", "TIX");
			break;
		}
		case OPCODE_JEQ: {
			printf("%6s", "JEQ");
			break;
		}
		case OPCODE_JGT: {
			printf("%6s", "JGT");
			break;
		}
		case OPCODE_JLT: {
			printf("%6s", "JLT");
			break;
		}
		case OPCODE_J: {
			printf("%6s", "J");
			break;
		}
		case OPCODE_JSUB: {
			printf("%6s", "JSUB");
			break;
		}
		case OPCODE_RSUB: {
			printf("%6s", "RSUB");
			break;
		}
		case OPCODE_ADD: {
			printf("%6s", "ADD");
			break;
		}
		case OPCODE_SUB: {
			printf("%6s", "SUB");
			break;
		}
		case OPCODE_MUL: {
			printf("%6s", "MUL");
			break;
		}
		case OPCODE_DIV: {
			printf("%6s", "DIV");
			break;
		}
		case OPCODE_ADDF: {
			printf("%6s", "ADDF");
			break;
		}
		case OPCODE_SUBF: {
			printf("%6s", "SUBF");
			break;
		}
		case OPCODE_MULF: {
			printf("%6s", "MULF");
			break;
		}
		case OPCODE_DIVF: {
			printf("%6s", "DIVF");
			break;
		}
		case OPCODE_RD: {
			printf("%6s", "RD");
			break;
		}
		case OPCODE_WD: {
			printf("%6s", "WD");
			break;
		}
		case OPCODE_TD: {
			printf("%6s", "TD");
			break;
		}
		case OPCODE_SSK: {
			printf("%6s", "SSK");
			break;
		}
		default:
			return 1;
	}
	if(!(flags & 1))
		operand = s12(operand);

	printf(" ");
	printf(ni == 1 ? "#" : (ni == 2 ? "@" : " "));
	if(flags & 2)
		printf(" PC");
	if(flags & 4)
		printf(" B");
	if(flags & 8)
		printf(" X");

	printf(" %ld", operand);
	return 0;
}
int disasm_instr(const unsigned char *memory) {
	unsigned char opcode;
	
	opcode = memory[0];

	int retcode;
	/* Format 1 */
	retcode = disasmF1(opcode);
	if(retcode  < 0) return -1;
	if(retcode == 0) return 1;
	
	long operand;

	operand = memory[1];

	/* Format 2 */
	retcode = disasmF2(opcode, operand);
	if(retcode  < 0) return -1;
	if(retcode == 0) return 2;

	int ni = opcode & 3;
	opcode &= 0xFC;
	
	operand = (operand << 8) | memory[2];

	int flags;
	if(ni == 0) {
		flags = operand & 0x8000;
		operand &= 0x7FFF;
	}
	else {
		flags = (operand & 0xF000) >> 12;
		operand &= 0x0FFF;
		if(flags & 1) {
			operand = (operand << 8) | memory[3];
		}
	}

	operand &= 0xFFFFFF;
	retcode = disasmSICF3F4(opcode, ni, operand, flags);
	if(retcode  < 0) return -1;
	if(retcode == 0) return 3 + (flags & 1);

	return 0;
}

int disasm(const unsigned char *memory, long start_addr, long num) {
	int i;
	long addr = 0;
	for(i=0; i<num; i++) {
		printf("0x%06lX: ", start_addr+addr);
		int instr_len = disasm_instr(&memory[addr]);
		if(instr_len < 0)
			break;
		addr += instr_len;

		printf("\n");
	}
	return 0;
}
