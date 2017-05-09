#include "Node.h"

#include "Code.h"
#include "Machine.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int is_e(struct Node *node) {
	return node->nixbpe & 1;
}
int is_pc(struct Node *node) {
	return node->nixbpe & 2;
}
int is_b(struct Node *node) {
	return node->nixbpe & 4;
}
int is_x(struct Node *node) {
	return node->nixbpe & 8;
}

int parse_data(struct Node *node, char *data) {
	int len = strlen(data);
	if(node->type == NT_STORAGE && (node->mnemonic == STORAGE_WORD || node->mnemonic == STORAGE_BYTE)) {
		if(len > 2 && data[0] == 'C' && data[1] == '\'') {
			char *start = data + 2;
			char *end = strchr(start, '\'');
			if(end == NULL)
				return -1;
			int len = end - start;
			*end = '\0';
			node->data = (char *)malloc(len);

			strncpy(node->data, start, len);
			node->size = len;
			if(node->mnemonic == STORAGE_WORD)
				node->size += node->size % 3;
			return 0;
		}
		else {
			long value = strtol(data, NULL, 0);
			if(node->mnemonic == STORAGE_BYTE) {
				node->data = (char *)malloc(1);
				*(node->data) = (char)(value&0xFF);
				node->size = 1;
			}
			else {
				node->data = (char *)malloc(3);
				*(node->data) = (char)( (value&0xFF0000) >> 16);
				*(node->data+1) = (char)( (value&0xFF00) >> 8);
				*(node->data+2) = (char)(value&0xFF);
				node->size = 3;
			}

		}
	}
	else if(node->type == NT_STORAGE && (node->mnemonic == STORAGE_RESW || node->mnemonic == STORAGE_RESB)) {
		long value = strtol(data, NULL, 0);
		node->size = value;
		if(node->mnemonic == STORAGE_RESW)
			node->size *= 3;
	}

	return 0;
}

int Node_init(struct Node *node, char *tokens[], int token_count, int comment, long loc) {
	node->type = NT_NULL;
	node->label[0] = '\0';
	node->mnemonic = OPCODE_INVALID;
	node->extended = 0;
	node->operands[0] = '\0';
	node->operands_vanilla[0] = '\0';
	node->nixbpe = 0;
	node->op = 0;
	node->comment[0] = '\0';
	node->size = 0;
	node->next = NULL;

	node->loc = loc;

	if(token_count < 1)
		return -1;
	if(comment == 0) {
		node->type = NT_COMMENT;
		strncpy(node->comment, tokens[comment], sizeof(node->comment));
		node->size = 0;

		return 0;
	}
	if(token_count < 2)
		return -1;

	strncpy(node->label, tokens[0], sizeof(node->label));

	node->mnemonic = opcode_parseMnemonic(tokens[1]);
	if(node->mnemonic == OPCODE_EXTENDED) {
		node->extended = 1;
		node->mnemonic = opcode_parseMnemonic(tokens[1]+1);
	}

	node->size = opcode_getFormat(node->mnemonic);
	switch(node->size) {
		case 0:
			node->type = NT_DIRECTIVE;
			break;
		case 1:
			node->type = NT_INSTF1;
			break;
		case 2:
			node->type = NT_INSTF2;
			break;
		case 3:
			node->type = NT_INSTF3;
			break;
	}

	strncpy(node->operands_vanilla, tokens[2], sizeof(node->operands_vanilla));

	if(node->size <= 3) {
		strncpy(node->operands, tokens[2], sizeof(node->operands));
		node->size += node->extended;
	}
	else if(node->size == 5) {
		node->type = NT_STORAGE;
		parse_data(node, tokens[2]);
	}

	if(comment > -1)
		strncpy(node->comment, tokens[comment], sizeof(node->comment));
	
	if(node->mnemonic == DIRECTIVE_START || node->mnemonic == DIRECTIVE_ORG) {
			node->op = strtol(node->operands, NULL, 0);
			node->loc = node->op;
	}
	else if(node->mnemonic == DIRECTIVE_EQU) {
			node->op = strtol(node->operands, NULL, 0);
	}

	if(node->size < 0)
		node->size = 0;
	
	return 0;
}

enum reg char_to_reg(char R) {
	switch(R) {
		case 'A':
			return REG_A;
		case 'X':
			return REG_X;
		case 'L':
			return REG_L;
		case 'B':
			return REG_B;
		case 'S':
			return REG_S;
		case 'T':
			return REG_T;
		case 'F':
			return REG_F;
	}
}
int Node_operandResolution(struct Node *node, struct Code *code) {
	if(node->type == NT_INSTF2) {
		char *op1 = NULL, *op2=NULL;
		op1 = node->operands;
		node->operands[1] = '\0';
		op2 = node->operands+2;

		int v1 = char_to_reg(*op1);
		int v2 = 0;
		if(isdigit(*op2)) {
			v2 = strtol(op2, NULL, 0);
		}
		else if(*op2 != '\0'){
			v2 = char_to_reg(*op2);
		}
		else
			v2 = 0;
		if(node->mnemonic == OPCODE_SHIFTL || node->mnemonic == OPCODE_SHIFTL)
			v2--;
		node->op = (v1 << 4) | (v2 & 0xFF);

		return 0;
	}
	else if(node->type == NT_INSTF3) {
		if(node->mnemonic == OPCODE_RSUB) {
			return 0;
		}
		
		char *op1 = node->operands;
		char *op2 = strchr(node->operands, ',');
		if(op2 != NULL) {
			*op2 = '\0';
			op2++;
		}

		if(op1[0] == '#') {
			node->nixbpe |= 1 << 4;
			op1++;
		}
		else if(op1[0] == '@') {
			node->nixbpe |= 1 << 5;
			op1++;
		}
		else {
			node->nixbpe |= 3 << 4;
		}

		if(isdigit(*op1)) {
			node->op = strtol(op1, NULL, 0) & 0x0FFFFF;
		}
		else {
			long pc = node->loc + node->size;
			long addr = Symtab_get(code->symtab, op1);

			long diff = addr - pc;
			if(((node->nixbpe & 0x30) != 0x10) && (diff >= -2048) && (diff < 2048)) {
				node->nixbpe |= 1 << 1;
				node->op = diff;
			}
			else {
				node->op = addr;
			}
 		}

		if(op2 != NULL && strcmp(op2, "X") == 0) {
			node->nixbpe |= 1 << 3;
		}

		return 0;
	}
	else if(node->mnemonic == DIRECTIVE_END) {
		char *op = node->operands;
		node->op = Symtab_get(code->symtab, op);
		return 0;
	}
	else
		return 0;
}

int Node_emitText(struct Node *node, unsigned char *buf, int size) {
	if(node->type == NT_INSTF1) {
		buf[0] = node->mnemonic;
	}
	else if(node->type == NT_INSTF2) {
		buf[0] = node->mnemonic;
		buf[1] = node->op & 0xFF;
	}
	else if(node->type == NT_INSTF3) {
		buf[0] = node->mnemonic;
		buf[0] |= (node->nixbpe & 0x30) >> 4;
		buf[1] = (node->nixbpe << 4) & 0xF0;
		if(!node->extended) {
			buf[1] |= (node->op & 0xF00) >> 8;
			buf[2] = node->op & 0xFF;
		}
		else {
			buf[1] |= (node->op & 0xF0000) >> 16;
			buf[2] = (node->op & 0xFF00) >> 8;
			buf[3] = node->op & 0xFF;
		}
	}
	else if(node->type == NT_STORAGE) {
		if(node->mnemonic == STORAGE_BYTE || node->mnemonic == STORAGE_WORD)
			memcpy(buf, node->data, node->size);
	}
	return node->size;
}

int Node_printNode(struct Node *node) {
	printf("==NODE==================\n");
	printf("label: %s\n", node->label);
	printf("mnemonic: %X\n", node->mnemonic);
	printf("operands: %s\n", node->operands);
	printf("nixbpe: %X\n", node->nixbpe);
	printf("op: %X\n", node->op);
	printf("size: %d\n", node->size);
	printf("========================\n");
}
