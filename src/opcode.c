#include "opcode.h"

#include <stdio.h>
#include <string.h>

char *opcode_str[] = {
	"LDA", "LDX", "LDL", "STA", "STX", "STL", "ADD", "SUB",
	"MUL", "DIV", "COMP", "TIX", "JEQ", "JGT", "JLT", "J",
	"AND", "OR", "JSUB", "RSUB", "LDCH", "STCH", "ADDF", "SUBF",
	"MULF", "DIVF", "LDB", "LDS", "LDF", "LDT", "STB", "STS",
	"STF", "STT", "COMPF", NULL, "ADDR", "SUBR", "MULR", "DIVR",
	"COMPR", "SHIFTL", "SHIFTR", "RMO", "SVC", "CLEAR", "TIXR", NULL,
	"FLOAT", "FIX", "NORM", NULL, "LPS", "STI", "RD", "WD",
	"TD", NULL, "STSW", "SSK", "SIO", "HIO", "TIO", NULL, 
};

int opcode_to_str(enum opcode opcode, char *str, int n) {
	if(opcode < 0x100) {
		if(opcode_str[opcode>>2] != NULL)
			strncpy(str, opcode_str[opcode>>2], n);
	}
	else {
		switch(opcode) {
			case DIRECTIVE_START:
				strncpy(str, "START", n);
				break;
			case DIRECTIVE_END:
				strncpy(str, "END", n);
				break;
			case DIRECTIVE_ORG:
				strncpy(str, "ORG", n);
				break;
			case DIRECTIVE_EQU:
				strncpy(str, "EQU", n);
				break;
			case DIRECTIVE_BASE:
				strncpy(str, "BASE", n);
				break;
			case DIRECTIVE_NOBASE:
				strncpy(str, "NOBASE", n);
				break;
			case DIRECTIVE_LTORG:
				strncpy(str, "LTORG", n);
				break;
			case STORAGE_BYTE:
				strncpy(str, "BYTE", n);
				break;
			case STORAGE_WORD:
				strncpy(str, "WORD", n);
				break;
			case STORAGE_RESB:
				strncpy(str, "RESB", n);
				break;
			case STORAGE_RESW:
				strncpy(str, "RESW", n);
				break;
			case STORAGE_FLOAT:
				strncpy(str, "FLOAT", n);
				break;
		}
	}
}

enum opcode opcode_parseMnemonic(char *mnemonic) {
	if(mnemonic[0] == '+')
		return OPCODE_EXTENDED;
	else if(strcmp(mnemonic, "LDA") == 0)
		return OPCODE_LDA;
	else if(strcmp(mnemonic, "LDCH") == 0)
		return OPCODE_LDCH;
	else if(strcmp(mnemonic, "LDX") == 0)
		return OPCODE_LDX;
	else if(strcmp(mnemonic, "LDL") == 0)
		return OPCODE_LDL;
	else if(strcmp(mnemonic, "LDB") == 0)
		return OPCODE_LDB;
	else if(strcmp(mnemonic, "LDS") == 0)
		return OPCODE_LDS;
	else if(strcmp(mnemonic, "LDF") == 0)
		return OPCODE_LDF;
	else if(strcmp(mnemonic, "LDT") == 0)
		return OPCODE_LDT;
	else if(strcmp(mnemonic, "LPS") == 0)
		return OPCODE_LPS;
	else if(strcmp(mnemonic, "STA") == 0)
		return OPCODE_STA;
	else if(strcmp(mnemonic, "STCH") == 0)
		return OPCODE_STCH;
	else if(strcmp(mnemonic, "STX") == 0)
		return OPCODE_STX;
	else if(strcmp(mnemonic, "STL") == 0)
		return OPCODE_STL;
	else if(strcmp(mnemonic, "STB") == 0)
		return OPCODE_STB;
	else if(strcmp(mnemonic, "STS") == 0)
		return OPCODE_STS;
	else if(strcmp(mnemonic, "STF") == 0)
		return OPCODE_STF;
	else if(strcmp(mnemonic, "STT") == 0)
		return OPCODE_STT;
	else if(strcmp(mnemonic, "STI") == 0)
		return OPCODE_STI;
	else if(strcmp(mnemonic, "STSW") == 0)
		return OPCODE_STSW;
	else if(strcmp(mnemonic, "CLEAR") == 0)
		return OPCODE_CLEAR;
	else if(strcmp(mnemonic, "RMO") == 0)
		return OPCODE_RMO;
	else if(strcmp(mnemonic, "AND") == 0)
		return OPCODE_AND;
	else if(strcmp(mnemonic, "OR") == 0)
		return OPCODE_OR;
	else if(strcmp(mnemonic, "SHIFTL") == 0)
		return OPCODE_SHIFTL;
	else if(strcmp(mnemonic, "SHIFTR") == 0)
		return OPCODE_SHIFTR;
	else if(strcmp(mnemonic, "COMP") == 0)
		return OPCODE_COMP;
	else if(strcmp(mnemonic, "COMPR") == 0)
		return OPCODE_COMPR;
	else if(strcmp(mnemonic, "COMPF") == 0)
		return OPCODE_COMPF;
	else if(strcmp(mnemonic, "TIX") == 0)
		return OPCODE_TIX;
	else if(strcmp(mnemonic, "TIXR") == 0)
		return OPCODE_TIXR;
	else if(strcmp(mnemonic, "JEQ") == 0)
		return OPCODE_JEQ;
	else if(strcmp(mnemonic, "JGT") == 0)
		return OPCODE_JGT;
	else if(strcmp(mnemonic, "JLT") == 0)
		return OPCODE_JLT;
	else if(strcmp(mnemonic, "J") == 0)
		return OPCODE_J;
	else if(strcmp(mnemonic, "JSUB") == 0)
		return OPCODE_JSUB;
	else if(strcmp(mnemonic, "RSUB") == 0)
		return OPCODE_RSUB;
	else if(strcmp(mnemonic, "ADD") == 0)
		return OPCODE_ADD;
	else if(strcmp(mnemonic, "SUB") == 0)
		return OPCODE_SUB;
	else if(strcmp(mnemonic, "MUL") == 0)
		return OPCODE_MUL;
	else if(strcmp(mnemonic, "DIV") == 0)
		return OPCODE_DIV;
	else if(strcmp(mnemonic, "ADDR") == 0)
		return OPCODE_ADDR;
	else if(strcmp(mnemonic, "SUBR") == 0)
		return OPCODE_SUBR;
	else if(strcmp(mnemonic, "MULR") == 0)
		return OPCODE_MULR;
	else if(strcmp(mnemonic, "DIVR") == 0)
		return OPCODE_DIVR;
	else if(strcmp(mnemonic, "ADDF") == 0)
		return OPCODE_ADDF;
	else if(strcmp(mnemonic, "SUBF") == 0)
		return OPCODE_SUBF;
	else if(strcmp(mnemonic, "MULF") == 0)
		return OPCODE_MULF;
	else if(strcmp(mnemonic, "DIVF") == 0)
		return OPCODE_DIVF;
	else if(strcmp(mnemonic, "FLOAT") == 0)
		return OPCODE_FLOAT;
	else if(strcmp(mnemonic, "FIX") == 0)
		return OPCODE_FIX;
	else if(strcmp(mnemonic, "NORM") == 0)
		return OPCODE_NORM;
	else if(strcmp(mnemonic, "RD") == 0)
		return OPCODE_RD;
	else if(strcmp(mnemonic, "WD") == 0)
		return OPCODE_WD;
	else if(strcmp(mnemonic, "TD") == 0)
		return OPCODE_TD;
	else if(strcmp(mnemonic, "SSK") == 0)
		return OPCODE_SSK;
	else if(strcmp(mnemonic, "SIO") == 0)
		return OPCODE_SIO;
	else if(strcmp(mnemonic, "HIO") == 0)
		return OPCODE_HIO;
	else if(strcmp(mnemonic, "TIO") == 0)
		return OPCODE_TIO;
	else if(strcmp(mnemonic, "SVC") == 0)
		return OPCODE_SVC;

	else if(strcmp(mnemonic, "START") == 0)
		return DIRECTIVE_START;
	else if(strcmp(mnemonic, "END") == 0)
		return DIRECTIVE_END;
	else if(strcmp(mnemonic, "ORG") == 0)
		return DIRECTIVE_ORG;
	else if(strcmp(mnemonic, "EQU") == 0)
		return DIRECTIVE_EQU;
	else if(strcmp(mnemonic, "BASE") == 0)
		return DIRECTIVE_BASE;
	else if(strcmp(mnemonic, "NOBASE") == 0)
		return DIRECTIVE_NOBASE;
	else if(strcmp(mnemonic, "LTORG") == 0)
		return DIRECTIVE_LTORG;

	else if(strcmp(mnemonic, "BYTE") == 0)
		return STORAGE_BYTE;
	else if(strcmp(mnemonic, "WORD") == 0)
		return STORAGE_WORD;
	else if(strcmp(mnemonic, "RESB") == 0)
		return STORAGE_RESB;
	else if(strcmp(mnemonic, "RESW") == 0)
		return STORAGE_RESW;
	else if(strcmp(mnemonic, "FLOAT") == 0)
		return STORAGE_FLOAT;

	else
		return OPCODE_INVALID;
}

int opcode_getFormat(enum opcode opcode) {
	switch(opcode) {
		case DIRECTIVE_START:
		case DIRECTIVE_END:
		case DIRECTIVE_ORG:
		case DIRECTIVE_EQU:
		case DIRECTIVE_BASE:
		case DIRECTIVE_NOBASE:
		case DIRECTIVE_LTORG:
			return 0;

		case OPCODE_FLOAT:
		case OPCODE_FIX:
		case OPCODE_NORM:
		case OPCODE_SIO:
		case OPCODE_HIO:
		case OPCODE_TIO:
			return 1;

		case OPCODE_ADDR:
		case OPCODE_SUBR:
		case OPCODE_MULR:
		case OPCODE_DIVR:
		case OPCODE_COMPR:
		case OPCODE_SHIFTL:
		case OPCODE_SHIFTR:
		case OPCODE_RMO:
		case OPCODE_SVC:
		case OPCODE_CLEAR:
		case OPCODE_TIXR:
			return 2;

		case OPCODE_LDA:
		case OPCODE_LDCH:
		case OPCODE_LDX:
		case OPCODE_LDL:
		case OPCODE_LDB:
		case OPCODE_LDS:
		case OPCODE_LDF:
		case OPCODE_LDT:

		case OPCODE_LPS:

		case OPCODE_STA:
		case OPCODE_STCH:
		case OPCODE_STX:
		case OPCODE_STL:
		case OPCODE_STB:
		case OPCODE_STS:
		case OPCODE_STF:
		case OPCODE_STT:

		case OPCODE_STI:
		case OPCODE_STSW:

		case OPCODE_AND:
		case OPCODE_OR:

		case OPCODE_COMP:
		case OPCODE_COMPF:

		case OPCODE_TIX:

		case OPCODE_JEQ:
		case OPCODE_JGT:
		case OPCODE_JLT:
		case OPCODE_J:

		case OPCODE_JSUB:
		case OPCODE_RSUB:

		case OPCODE_ADD:
		case OPCODE_SUB:
		case OPCODE_MUL:
		case OPCODE_DIV:

		case OPCODE_ADDF:
		case OPCODE_SUBF:
		case OPCODE_MULF:
		case OPCODE_DIVF:

		case OPCODE_RD:
		case OPCODE_WD:
		case OPCODE_TD:

		case OPCODE_SSK:
			return 3;

		case STORAGE_BYTE:
		case STORAGE_WORD:
		case STORAGE_RESB:
		case STORAGE_RESW:
		case STORAGE_FLOAT:
			return 5;
	}
	return -1;
}
